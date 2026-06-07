# =============================================================================
# Embedded-Base integration layer for the cdefmt logging library
# (https://github.com/RisinT96/cdefmt).
#
# Goal: a parent project should be able to enable cdefmt with no edits to its
#       CubeMX-managed linker script or toolchain file. Everything that cdefmt
#       requires (compile flag, link flag, extra sections in the ELF, default
#       config header) is propagated through a single INTERFACE target.
#
# Usage (from the parent project's CMakeLists.txt):
#
#   add_subdirectory(Drivers/Embedded-Base/cdefmt_port)
#   target_link_libraries(${CMAKE_PROJECT_NAME} embedded_base_cdefmt)
#
# Then the parent project must:
#   1. Provide an implementation of `void cdefmt_log(const void*, size_t,
#      enum cdefmt_level)` in one of its source files. This is the transport
#      sink (e.g. UART, ITM/SWO, RTT, ...).
#   2. Place `CDEFMT_GENERATE_INIT();` at file scope exactly once (typically
#      in the same source file that implements `cdefmt_log`).
#   3. Call `cdefmt_init()` once after the transport is up, before any logs
#      are emitted.
#
# Overriding the default config:
#   This package adds `<this-dir>/include/` to the include path, which gives
#   `<config/cdefmt_config.h>` a sensible default. To override, the parent
#   project should arrange for an earlier include directory to provide its
#   own `config/cdefmt_config.h`.
# =============================================================================

cmake_minimum_required(VERSION 3.22)

set(_CDEFMT_PORT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(_CDEFMT_UPSTREAM_DIR ${_CDEFMT_PORT_DIR}/cdefmt)

if(NOT EXISTS ${_CDEFMT_UPSTREAM_DIR}/CMakeLists.txt)
    message(FATAL_ERROR
        "cdefmt_port: upstream cdefmt submodule not found at "
        "${_CDEFMT_UPSTREAM_DIR}. Did you `git submodule update --init "
        "--recursive`?")
endif()

# Pull in upstream cdefmt. Its CMakeLists only builds the example/decoder
# targets when it is the top-level project, so being added as a subdirectory
# is a no-op aside from defining the `cdefmt` INTERFACE library and fetching
# the two header-only Boost dependencies.
if(NOT TARGET cdefmt)
    add_subdirectory(
        ${_CDEFMT_UPSTREAM_DIR}
        ${CMAKE_BINARY_DIR}/cdefmt
        EXCLUDE_FROM_ALL
    )
endif()

# Linker script fragment that defines the `.cdefmt` metadata section and the
# `.note.gnu.build-id` section. It is passed as an additional `-T` argument;
# GNU ld merges multiple `-T` scripts, so this layers cleanly on top of the
# main linker script without replacing it.
set(_CDEFMT_LD_FRAGMENT ${_CDEFMT_PORT_DIR}/cdefmt_sections.ld)

add_library(embedded_base_cdefmt INTERFACE)

target_link_libraries(embedded_base_cdefmt INTERFACE cdefmt)

target_include_directories(embedded_base_cdefmt INTERFACE
    ${_CDEFMT_PORT_DIR}/include
)

# Treat cdefmt's header and its Boost.Preprocessor/VMD dependencies as system
# headers for anyone who links `embedded_base_cdefmt`. cdefmt's logging macros
# expand into code that legitimately trips -Wunused-variable,
# -Wdiscarded-qualifiers and -Wunused-function; GCC attributes those warnings to
# the macro body (in cdefmt.h), so promoting these include dirs to `-isystem`
# silences them at the source without touching the parent project's build files
# or suppressing warnings in the parent's own code.
#
# We can't simply mark them SYSTEM here because the include dirs are propagated
# by the upstream `cdefmt`/`Boost::*` targets, which we don't own. Instead we
# copy each target's INTERFACE_INCLUDE_DIRECTORIES into its
# INTERFACE_SYSTEM_INCLUDE_DIRECTORIES, the portable pre-CMake-3.25 way to do
# this (the `SYSTEM` keyword on add_subdirectory/targets needs >= 3.25).
foreach(_cdefmt_dep cdefmt Boost::preprocessor Boost::vmd)
    if(TARGET ${_cdefmt_dep})
        # Boost::* are ALIAS targets; set_property() rejects aliases, so resolve
        # to the underlying real target before touching its properties.
        get_target_property(_cdefmt_real ${_cdefmt_dep} ALIASED_TARGET)
        if(NOT _cdefmt_real)
            set(_cdefmt_real ${_cdefmt_dep})
        endif()
        get_target_property(_cdefmt_dep_includes
            ${_cdefmt_real} INTERFACE_INCLUDE_DIRECTORIES)
        if(_cdefmt_dep_includes)
            set_property(TARGET ${_cdefmt_real} APPEND PROPERTY
                INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${_cdefmt_dep_includes})
        endif()
    endif()
endforeach()

# cdefmt's decoder needs DWARF info to parse argument types.
target_compile_options(embedded_base_cdefmt INTERFACE -g)

# Link-time requirements:
#   --build-id    : tag the ELF with a GNU build-id. cdefmt uses it at
#                   runtime to validate that the binary matches the ELF the
#                   decoder was given.
#   -T <fragment> : add the `.cdefmt` / `.note.gnu.build-id` sections. This
#                   does NOT replace the main linker script -- GNU ld merges
#                   multiple `-T` scripts.
target_link_options(embedded_base_cdefmt INTERFACE
    "LINKER:--build-id"
    "-T${_CDEFMT_LD_FRAGMENT}"
)
