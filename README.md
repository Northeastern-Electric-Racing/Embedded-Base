# Embedded-Base
Embedded-Base is a collection of drivers, middlewares, and communication definitions designed for use across various systems.  

Most firmware projects define Embedded-Base as a submodule.

## Getting Started With NER Firmware

All you need is on confluence [here](https://nerdocs.atlassian.net/wiki/spaces/NER/pages/1343533/Firmware+Onboarding+Embedded+Software#Development-Environment-Setup).

### Notable items in this repository

- `cangen`: All JSON definitions of in-car CAN data as well as documentation and parsing utilities.
- `dev`: The manual version of the NER build system, compatible up circuit boards in competition 22A
- `ner_environment`: The NER developer environment compatible with all boards from 22A onwards and cross platform support.
- `general`: Cross-platform C source files for all peripheral drivers used by NER.
- `middleware`: Cross-platform C source files common utilities used across various platforms.
- `platforms`: C source files specific to STM HAL versions.
- `clang-format`:  The organization wide clang-format definition.
- `ftdi_flash.cfg`: An openocd script to assist in FTDI flashing.
- `openocd.cfg`: A openocd stub to load GDB onto a target.


