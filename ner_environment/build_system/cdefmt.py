import glob
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path
import serial
import serial.tools.list_ports
from rich import print


CDEFMT_REPO_REL = Path("Drivers/Embedded-Base/cdefmt_port/cdefmt")
_DECODER_NAME = "stdin.exe" if platform.system() == "Windows" else "stdin" # cargo emits `stdin.exe` on Windows, `stdin` everywhere else.
DECODER_BIN_REL = CDEFMT_REPO_REL / "target" / "release" / _DECODER_NAME

# USB vendor IDs used to rank auto-detected adapters.
_VID_FTDI = 0x0403    # FTDI
_VID_STLINK = 0x0483  # ST-LINK


# ------------------------------------------------------------------------------
# Environment helpers
# ------------------------------------------------------------------------------

def _is_wsl() -> bool:
    """True when running under WSL, where USB serial devices must be attached
    with usbipd before they're visible. Mirrors serial2.py / build_system.py."""
    return (platform.system() == "Linux"
            and "microsoft" in platform.uname().release.lower())


def _print_wsl_usbip_hint() -> None:
    """Print the same usbipd guidance serial2.py shows when no device is found
    under WSL."""
    if not _is_wsl():
        return
    print("\n[blue]If you're using WSL, you may need to attach the USB device "
          "from a Windows terminal:")
    print("1. Open a Windows terminal [bold blue]with admin privileges[/bold blue] "
          "and run 'usbipd list'.")
    print("2. Find your device (often named 'CMSIS-DAP v2 Interface' or 'USB "
          "Serial Device').")
    print("3. Note its BUSID and run [bold green]'usbipd bind "
          "--busid=<BUSID>'[/bold green]. (Skip if already bound.)")
    print("4. Then run [bold green]'usbipd attach --wsl=ubuntu --busid "
          "<BUSID>'[/bold green].")
    print("5. Re-run the command.\n")


# ------------------------------------------------------------------------------
# Decoder discovery / build
# ------------------------------------------------------------------------------

def _find_decoder(rebuild: bool = False) -> Path:
    """Return the path to the host-side decoder binary.

    Builds it on first use (or when --rebuild is passed) by running
    `cargo build --release` in the cdefmt submodule. Exits with a helpful
    error if the submodule isn't checked out or if cargo isn't installed.
    """
    cdefmt_repo = Path.cwd() / CDEFMT_REPO_REL
    decoder = Path.cwd() / DECODER_BIN_REL

    if not cdefmt_repo.exists():
        print(f"[bold red]Error:[/bold red] cdefmt submodule not found at "
              f"[blue]{CDEFMT_REPO_REL}[/blue]. Did you run "
              f"`git submodule update --init --recursive`?", file=sys.stderr)
        sys.exit(1)

    if decoder.exists() and not rebuild:
        return decoder

    if shutil.which("cargo") is None:
        print("[bold red]Error:[/bold red] `cargo` is not on PATH. Install "
              "Rust (https://rustup.rs) to build the cdefmt host-side "
              "decoder.", file=sys.stderr)
        sys.exit(1)

    action = "Rebuilding" if rebuild else "Building"
    print(f"[#cccccc](ner cdefmt):[/#cccccc] [blue]{action} host-side "
          f"decoder...[/blue]")
    result = subprocess.run(["cargo", "build", "--release"], cwd=cdefmt_repo)
    if result.returncode != 0:
        print("[bold red]Error:[/bold red] `cargo build` failed; see output "
              "above.", file=sys.stderr)
        sys.exit(result.returncode)

    if not decoder.exists():
        # Cargo succeeded but the expected binary isn't where we look for it.
        # Most likely the upstream cdefmt repo restructured its `examples/`.
        print(f"[bold red]Error:[/bold red] cargo build succeeded but the "
              f"decoder binary was not produced at [blue]{decoder}[/blue]. "
              f"Has the cdefmt example layout changed upstream?",
              file=sys.stderr)
        sys.exit(1)

    return decoder


# ------------------------------------------------------------------------------
# ELF discovery
# ------------------------------------------------------------------------------

def _find_elf(explicit: str | None) -> Path:
    """Return the ELF to decode against -- explicit path if given, else the
    most-recently-modified .elf under build/."""
    if explicit:
        p = Path(explicit)
        if not p.is_file():
            print(f"[bold red]Error:[/bold red] ELF not found: "
                  f"[blue]{p}[/blue]", file=sys.stderr)
            sys.exit(1)
        return p

    candidates = glob.glob("build/*.elf")
    if not candidates:
        print("[bold red]Error:[/bold red] no ELF found under build/. Run "
              "`ner build` first, or pass --elf.", file=sys.stderr)
        sys.exit(1)

    # Newest by mtime, so multi-target projects pick whatever was just rebuilt.
    return Path(max(candidates, key=os.path.getmtime))


# ------------------------------------------------------------------------------
# Serial port discovery
# ------------------------------------------------------------------------------

def _device_priority(port: "serial.tools.list_ports_common.ListPortInfo") -> int:
    desc = " ".join(filter(None, [
        port.description, port.product, port.manufacturer, port.interface,
    ])).lower()

    # Prioritize FTDI
    if port.vid == _VID_FTDI or "ftdi" in desc:
        return 0
    if "cmsis-dap" in desc or "daplink" in desc or "mbed" in desc:
        return 1
    if port.vid == _VID_STLINK or "st-link" in desc or "stlink" in desc:
        return 3
    return 2


def _list_ports() -> list["serial.tools.list_ports_common.ListPortInfo"]:
    """Return USB serial devices visible to the OS, preferred adapter first."""
    ports = [p for p in serial.tools.list_ports.comports() if p.vid is not None]
    ports.sort(key=lambda p: (_device_priority(p), p.device))
    return ports


def _find_port(explicit: str) -> str:
    """Return the serial port to use (if nothing is passed in for `explicit`, auto-detect)"""
    if explicit:
        return explicit

    ports = _list_ports()
    if not ports:
        print("[bold red]Error:[/bold red] no USB serial devices found. Is "
              "the board plugged in?", file=sys.stderr)
        _print_wsl_usbip_hint()
        sys.exit(1)

    return ports[0].device


# ------------------------------------------------------------------------------
# Entry point
# ------------------------------------------------------------------------------

def main(device: str = "", elf: str | None = None, rebuild: bool = False,
         ls: bool = False, baud: int = 115200) -> None:
    """Invoked by the `ner cdefmt` command. See cdefmt() in build_system.py."""

    if ls:
        ports = _list_ports()
        elfs = sorted(glob.glob("build/*.elf"))
        print("[bold]Serial ports:[/bold]")
        if ports:
            for p in ports:
                desc = p.description if p.description and p.description != "n/a" else ""
                print(f"  {p.device}" + (f"  [#888888]({desc})[/#888888]" if desc else ""))
            print("[#888888]  The first port is selected if --device is not "
                  "given.[/#888888]")
        else:
            print("  (none)")
            _print_wsl_usbip_hint()
        print("[bold]ELF files under build/:[/bold]")
        for e in (elfs or ["  (none)"]):
            print(f"  {e}")
        return

    decoder = _find_decoder(rebuild=rebuild)
    elf_path = _find_elf(elf)
    port = _find_port(device)

    print(f"[#cccccc](ner cdefmt):[/#cccccc] decoding [blue]{port}[/blue] "
          f"against [blue]{elf_path}[/blue] -- Ctrl-C to quit")

    # Open the port with pyserial and pump its bytes into the decoder's stdin.
    # The decoder reads a length-prefixed binary stream, so we forward raw chunks as they arrive.
    proc = subprocess.Popen([str(decoder), "--elf", str(elf_path)],
                            stdin=subprocess.PIPE)
    interrupted = False
    try:
        with serial.Serial(port, baud, timeout=0.1) as ser:
            while proc.poll() is None:
                chunk = ser.read(ser.in_waiting or 1)
                if not chunk:
                    continue
                try:
                    proc.stdin.write(chunk)
                    proc.stdin.flush()
                except (BrokenPipeError, OSError):
                    break  # decoder exited, stop forwarding
    except serial.SerialException as e:
        print(f"[bold red]Error:[/bold red] could not open serial port "
              f"[blue]{port}[/blue]: {e}", file=sys.stderr)
        _print_wsl_usbip_hint()
        proc.terminate()
        proc.wait()
        sys.exit(1)
    except KeyboardInterrupt:
        interrupted = True
    finally:
        if proc.stdin and not proc.stdin.closed:
            try:
                proc.stdin.close()
            except OSError:
                pass

    try:
        rc = proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.terminate()
        rc = proc.wait()

    # Make a Ctrl-C-killed decoder indistinguishable from running it directly.
    sys.exit(130 if interrupted else rc)
