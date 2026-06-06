# cdefmt_port

Embedded-Base integration layer for [cdefmt](https://github.com/RisinT96/cdefmt),
a deferred-formatting logger inspired by Rust's [`defmt`](https://github.com/knurling-rs/defmt).

The goal of this directory is to make `cdefmt` drop-in for any Embedded-Base
consumer **without** touching the parent project's CubeMX-managed linker
script or toolchain file.

## What this provides

A single CMake `INTERFACE` target, `embedded_base_cdefmt`, that propagates:

| Requirement                       | How it's satisfied                                          |
| --------------------------------- | ----------------------------------------------------------- |
| Header-only `cdefmt` library      | `add_subdirectory(../cdefmt)` (pulls in Boost preprocessor + VMD via `FetchContent`) |
| `-g` compile flag                 | `target_compile_options(... -g)`                            |
| `-Wl,--build-id` link flag        | `target_link_options(... LINKER:--build-id)`                |
| `.cdefmt` + `.note.gnu.build-id` linker sections | Extra `-T cdefmt_sections.ld` passed to ld (ld merges multiple `-T` scripts, so the main linker script is **not** replaced) |
| `<config/cdefmt_config.h>`        | `include/config/cdefmt_config.h` added to include path      |

## Usage

In the parent project's top-level `CMakeLists.txt`:

```cmake
add_subdirectory(Drivers/Embedded-Base/cdefmt_port)

target_link_libraries(${CMAKE_PROJECT_NAME}
    embedded_base_cdefmt
    # ... other libs ...
)
```

Then in one of the parent project's source files (e.g. `Core/Src/cdefmt_backend.c`):

```c
#include <cdefmt/include/cdefmt.h>
#include "main.h"

extern UART_HandleTypeDef huart2;

/* Emits the static `cdefmt_init()` function. Must appear exactly once in the
 * program. */
CDEFMT_GENERATE_INIT();

/* Transport sink. Called by every CDEFMT_* macro. */
void cdefmt_log(const void *log, size_t size, enum cdefmt_level level)
{
    (void)level;
    uint64_t hdr = (uint64_t)size;
    HAL_UART_Transmit(&huart2, (uint8_t *)&hdr, sizeof hdr, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)log, (uint16_t)size, HAL_MAX_DELAY);
}

/* Plain-C wrapper around the static-inline cdefmt_init() so the rest of the
 * program can call it without pulling in <cdefmt/include/cdefmt.h>. */
void cdefmt_init_app(void) { (void)cdefmt_init(); }
```

And call `cdefmt_init_app()` once, after the transport is initialized.

## Why this design

The `.cdefmt` and `.note.gnu.build-id` sections normally have to be added to
the project's main linker script. STM32CubeMX (and similar IDE-driven flows)
*regenerate* that linker script on every export, which would wipe manual
edits. By shipping a separate `cdefmt_sections.ld` fragment and passing it as
an additional `-T` argument, those sections are merged in by `ld` itself at
link time — so the CubeMX-managed `.ld` and toolchain file remain untouched.

The same idea applies to `-g` and `-Wl,--build-id`: rather than editing the
CubeMX toolchain file, they ride along on the `INTERFACE` target.

## Decoding logs on the host

The recommended way is to use the dedicated `ner` command, which lives in
[`ner_environment/build_system/cdefmt.py`](../ner_environment/build_system/cdefmt.py):

```bash
ner cdefmt                       # auto-detects port + ELF, builds decoder on first run
ner cdefmt --device /dev/ttyACM1 # specific port
ner cdefmt --elf path/to/fw.elf  # specific ELF
ner cdefmt --rebuild             # force rebuild of the host-side decoder
ner cdefmt --list                # list candidate ports / ELFs and exit
```

Under the hood the command:

1. Builds the Rust decoder at `Drivers/Embedded-Base/cdefmt/target/release/stdin`
   on first invocation (or when `--rebuild` is passed). Requires `cargo` on
   PATH — install Rust via <https://rustup.rs> if needed.
2. Picks the newest `build/*.elf` unless `--elf` overrides.
3. Picks the first `/dev/ttyACM*` or `/dev/ttyUSB*` unless `--device`
   overrides (Linux only; macOS/Windows users must pass `--device`).
4. Puts the TTY in `raw -echo -ixon -icrnl` mode so binary cdefmt frames
   pass through untouched (the kernel's default canonical line discipline
   would buffer until a `\n` arrives, which a binary protocol may never
   emit).
5. Exec's the decoder with the serial port redirected to its stdin.

If you'd rather invoke the decoder by hand (e.g. when running on a host that
doesn't have the `ner` venv installed), the equivalent shell incantation is:

```bash
cd Drivers/Embedded-Base/cdefmt
cargo build --release
stty -F /dev/ttyACM0 raw -echo -ixon -icrnl 115200
target/release/stdin --elf ../../../build/F4-Nucleo.elf < /dev/ttyACM0
```

The frame format that the bundled decoder expects is a 64-bit little-endian
length followed by that many bytes of payload — which is exactly what the
backend snippet above emits.

## Thread-safety note

`cdefmt_log` is called from whatever context invokes a `CDEFMT_*` macro. If
that includes multiple RTOS threads (or ISRs), wrap the body of `cdefmt_log`
in a mutex, or hand the frame off to a dedicated logging thread via a queue.
The integration here intentionally stays transport-agnostic and leaves this
to the consumer.

## Mixing transports

Do **not** interleave plain `printf` and `CDEFMT_*` output on the same UART.
cdefmt sends length-prefixed binary frames; any stray ASCII byte will be
interpreted by the host decoder as a (huge) length and crash it with
`memory allocation of <huge number> bytes failed`. Either:

- Send cdefmt and printf over different transports (e.g. cdefmt over ITM/SWO,
  printf over UART), or
- Drop printf entirely and use only cdefmt on the shared UART.

## Known cosmetic warning from STM32CubeProgrammer

When flashing the ELF you may see:

```
Warning: File corrupted. Two or more segments define the same memory zone
         or wrong order of segments
```

This is harmless. It is caused by the CubeMX-generated linker script
declaring `.bss` and `._user_heap_stack` as `(NOLOAD) >RAM` with no `AT`
clause, which makes ld auto-assign them a load address equal to the FLASH
location counter -- which happens to coincide with the address of the
auto-orphan-placed `.note.gnu.build-id` section. The conflicting segments
have `FileSiz = 0`, so no actual bytes are written or duplicated in flash;
CubeProgrammer just sees two PT_LOAD entries sharing a `PhysAddr` and
prints the warning regardless. Flashing completes successfully (`File
download complete`).

The only way to silence this without touching the CubeMX-managed linker
script is a post-link `objcopy --change-section-lma` step. The
`embedded_base_cdefmt` target deliberately does not do this by default --
it's cosmetic and doesn't affect correctness.

## Validated on F4-Nucleo (June 2026)

Working end-to-end with the integration above:

- Init log (`cdefmt_init()` emits the build-id) decodes correctly.
- All three log levels exercised (`CDEFMT_INFO`, `CDEFMT_DEBUG`,
  `CDEFMT_WARNING`) — formatting, level names, source location all show up
  correctly host-side.
- Multi-arg logs work, including alt-form hex (`{:#x}`).
- Frame stream is stable for 100+ iterations back-to-back without drift.

## Known follow-up: timing primitives inside ThreadX threads

Unrelated to cdefmt but discovered during integration: in this project, both
`HAL_Delay()` and `tx_thread_sleep()` hang forever when called from a
ThreadX thread, because the IRQs that drive their tick sources (TIM6 for HAL,
SysTick handler's effect on the suspend list for ThreadX) are not being
delivered to thread context. PRIMASK and BASEPRI are both 0, the relevant
ISRs are linked, and `tx_time_get()` *does* increment — yet sleeping threads
are never resumed and `uwTick` stays at 0.

This is a CubeMX-for-ThreadX integration quirk, not a cdefmt issue. As a
workaround, the demo in `Core/Src/u_threads.c` uses a CPU busy-wait between
log emissions. See the file header comment there for the full diagnosis.

