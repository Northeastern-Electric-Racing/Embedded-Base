# cdefmt_port

This is a wrapper layer to integrate the `cdefmt` into Embedded-Base. `cdefmt` is a library someone made that provides similar functionality to the `defmt` Rust crate. Link: https://github.com/RisinT96/cdefmt

## Setting Up cdefmt

To set up CDEFMT logging in an existing project, there are a few steps you need to follow.

First, make sure the project has Embedded-Base as a submodule, on a commit that has cdefmt_port included. You'll need to run `git submodule update --init --recursive` to make sure the cdefmt library is pulled.

Then, add this to the parent project's top-level `CMakeLists.txt`:

```cmake
add_subdirectory(Drivers/Embedded-Base/cdefmt_port)

target_link_libraries(${CMAKE_PROJECT_NAME}
    embedded_base_cdefmt
    # ... other libs ...
)
```

After that, add the cdefmt routing code to your project's `main.c`:

```c
#include <cdefmt/include/cdefmt.h>

/* Emits the static-inline `cdefmt_init()` function used during start-up to
 * send the build-id init log. Must appear at file scope exactly once in the
 * whole program. */
CDEFMT_GENERATE_INIT();

/**
 * @brief Transport sink invoked by every CDEFMT_* macro.
 */
void cdefmt_log(const void *log, size_t size, enum cdefmt_level level)
{
    (void)level; /* Runtime level filtering can be added here if desired. */

    const uint64_t hdr = (uint64_t)size;

    HAL_UART_Transmit(&huart2, (uint8_t *)&hdr, sizeof hdr, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)log, (uint16_t)size, HAL_MAX_DELAY);
}
```
(Obviously, you should use whatever UART your project has available, rather than specifically `huart2`. Also, make sure nothing else is using this UART aside from cdefmt; if you also have printf() routed to this same UART or something, you'll get errors when trying to run `ner cdefmt`.)

Also, add `cdefmt_init()` to the `main()` function in your `main.c`, after your UART initialization code has ran.

# Usage

Now, you should be ready to use cdefmt. In any file where you want to log stuff, add `#include <cdefmt/include/cdefmt.h>` to the top. Then, you can use the following cdefmt macros:
```c
CDEFMT_ERROR("This is an error log.");
CDEFMT_WARNING("This is a warning log.");
CDEFMT_INFO("This is an info log.");
CDEFMT_DEBUG("This is a debug log.");
CDEFMT_VERBOSE("This is a verbose log.");
```

Here's an example of how formatting data works in these macros:
```c
CDEFMT_DEBUG("loop state: tick={} led_pin={:#x}, cool_float={}", tick, led_pin, cool_float);
// `tick` is a uint32_t, `led_pin` is a uint16_t that we want to display in hex, and `cool_float` is a float
```

That macro call produced this log when I ran the code and viewed logging output (via `ner cdefmt`):
```
[u_threads.c/vDefault():78] Debug   > loop state: tick=17 led_pin=0x20, cool_float=64.08844
```
As shown above, the logs are automatically prepended with the filename, function name, line number, and logging level (e.g., Debug, Warning, Error, etc) corresponding to the macro call.

# Other Important Stuff

- Using cdefmt requires cargo, since the decoder will need to get built on your device.
- It may be helpful to run `ner cdefmt --rebuild` if you run into issues, since this will rebuild the decoder on your device.
- Again, you NEED to give cdefmt its own dedicated UART (or just disable printf()-to-UART routing) for `ner cdefmt` to work. If `ner cdefmt` encounters non-defmt prints when trying to decode, it will fail.