import glob
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path

from rich import print


# Paths are resolved relative to the project working directory (i.e. the dir
# from which the user ran `ner cdefmt`). This matches how every other ner
# command works.
CDEFMT_REPO_REL = Path("Drivers/Embedded-Base/cdefmt")
DECODER_BIN_REL = CDEFMT_REPO_REL / "target" / "release" / "stdin"


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

def _list_ports() -> list[str]:
    """Return USB serial devices currently visible to the OS.

    Linux only for now -- mirrors miniterm.py's coverage. macOS/Windows users
    should pass --device explicitly until this grows cross-platform support.
    """
    if platform.system() != "Linux":
        return []
    return sorted(glob.glob("/dev/ttyACM*") + glob.glob("/dev/ttyUSB*"))


def _find_port(explicit: str) -> str:
    """Return the serial port to use -- explicit if given, else first detected.

    The first detected port is typically /dev/ttyACM0 (a Nucleo's ST-LINK VCP),
    which is also what `nflash` targets by default.
    """
    if explicit:
        if not os.path.exists(explicit):
            print(f"[bold red]Error:[/bold red] device not found: "
                  f"[blue]{explicit}[/blue]", file=sys.stderr)
            sys.exit(1)
        return explicit

    if platform.system() != "Linux":
        print("[bold red]Error:[/bold red] auto-detect port not implemented "
              "for this OS; please pass --device.", file=sys.stderr)
        sys.exit(1)

    ports = _list_ports()
    if not ports:
        print("[bold red]Error:[/bold red] no USB serial devices found. Is "
              "the board plugged in?", file=sys.stderr)
        sys.exit(1)

    return ports[0]


# ------------------------------------------------------------------------------
# TTY setup
# ------------------------------------------------------------------------------

def _set_raw_tty(port: str, baud: int) -> None:
    """Put the TTY in raw mode so binary cdefmt frames pass through untouched.

    The kernel's default line discipline is `icanon`, which only releases
    bytes to read() when a newline arrives. That's catastrophic for a
    length-prefixed binary protocol -- the decoder would block waiting for an
    0x0a that may legitimately never appear. `raw` (= !icanon !echo !isig
    !iexten ...) plus explicit -icrnl/-ixon disables every transformation
    that would mangle binary payload bytes.
    """
    if platform.system() != "Linux":
        # stty's syntax differs on macOS/BSDs; not implemented yet.
        return

    cmd = ["stty", "-F", port, "raw", "-echo", "-ixon", "-icrnl", str(baud)]
    result = subprocess.run(cmd)
    if result.returncode != 0:
        print(f"[bold red]Error:[/bold red] `stty` failed on [blue]{port}"
              f"[/blue].", file=sys.stderr)
        sys.exit(result.returncode)


# ------------------------------------------------------------------------------
# Entry point
# ------------------------------------------------------------------------------

def main(device: str = "", elf: str | None = None, rebuild: bool = False,
         ls: bool = False, baud: int = 115200) -> None:
    """Invoked by the `ner cdefmt` command. See cdefmt() in build_system.py
    for the typer-level option definitions."""

    if ls:
        ports = _list_ports()
        elfs = sorted(glob.glob("build/*.elf"))
        print("[bold]Serial ports:[/bold]")
        for p in (ports or ["  (none)"]):
            print(f"  {p}")
        print("[bold]ELF files under build/:[/bold]")
        for e in (elfs or ["  (none)"]):
            print(f"  {e}")
        return

    decoder = _find_decoder(rebuild=rebuild)
    elf_path = _find_elf(elf)
    port = _find_port(device)

    _set_raw_tty(port, baud)

    print(f"[#cccccc](ner cdefmt):[/#cccccc] decoding [blue]{port}[/blue] "
          f"against [blue]{elf_path}[/blue] -- Ctrl-C to quit")

    # Hand the open port to the decoder as its stdin. Python stays in the
    # call stack just long enough to set this up; once subprocess.run returns
    # we propagate its exit code so a Ctrl-C-killed decoder is indistinguish-
    # able from running the binary directly.
    try:
        with open(port, "rb") as port_fd:
            result = subprocess.run(
                [str(decoder), "--elf", str(elf_path)],
                stdin=port_fd,
            )
    except KeyboardInterrupt:
        # The decoder will have already gotten the SIGINT; exit quietly.
        sys.exit(130)

    sys.exit(result.returncode)
