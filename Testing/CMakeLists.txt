# CMake to support NER Testing Framework
cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(PROJECT_DIR "/home/app")
set(TEST_DIR "${PROJECT_DIR}/Tests")
set(MOCKS_DIR "${TEST_DIR}/Mocks")
set(TEST_BUILD "${TEST_DIR}/build")

set(UNITY_SRC "/cmock_portable/vendor/unity/src")
set(CMOCK_SRC "/cmock_portable/src")

# NOTE: `TEST_FILE` is expected to be passed as a command line arg, e.g.:
#   cmake -DTEST_FILE=test_dcl_calcs.c ..

if(NOT DEFINED TEST_NAME)
    message(FATAL_ERROR "You must define TEST_NAME (e.g. -DTEST_NAME=test_dcl_calcs)")
endif()

if(NOT DEFINED TEST_FILE)
    message(FATAL_ERROR "You must define TEST_FILE (e.g. -DTEST_FILE=test_dcl_calcs.c)")
endif()


if(NOT DEFINED TEST_PACKAGE)
    message(FATAL_ERROR "You must define TEST_PACKAGE (e.g. -DTEST_PACKAGE=dcl_calcs)")
endif()

if(NOT DEFINED C_SOURCES)
    message(FATAL_ERROR "You must define C_SOURCES (e.g. -DC_SOURCES=Core/Src/analyzer.c ...)")
endif()

# Define project
project(${TEST_NAME})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TEST_BUILD}")

# Create an executable target
add_executable(${TEST_NAME})

# Add sources to the executable
target_sources(${TEST_NAME} PRIVATE
    ${CMOCK_SRC}/cmock.c
    ${UNITY_SRC}/unity.c
    ${PROJECT_DIR}/${TEST_FILE}
    ${C_SOURCES} 
)

# Add include paths (add any user-defined ones here)
target_include_directories(${TEST_NAME} PRIVATE
    # Add user defined include paths 
    ${INCLUDE_DIRS}
    ${MOCK_DIR}/${TEST_PACKAGE} # test package headers
    ${TEST_DIR}/Inc 
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)
