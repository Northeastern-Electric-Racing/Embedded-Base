cmake_minimum_required(VERSION 3.22)

set(_CDEFMT_PORT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(_CDEFMT_UPSTREAM_DIR ${_CDEFMT_PORT_DIR}/cdefmt)

if(NOT EXISTS ${_CDEFMT_UPSTREAM_DIR}/CMakeLists.txt)
    message(FATAL_ERROR
        "cdefmt_port: upstream cdefmt submodule not found at "
        "${_CDEFMT_UPSTREAM_DIR}. Did you `git submodule update --init "
        "--recursive`?")
endif()

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
# headers for anyone who links `embedded_base_cdefmt`.
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

# Link-time requirements
target_link_options(embedded_base_cdefmt INTERFACE
    "LINKER:--build-id"
    "-T${_CDEFMT_LD_FRAGMENT}"
)
