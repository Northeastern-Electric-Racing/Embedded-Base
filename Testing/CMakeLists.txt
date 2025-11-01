# CMake to support NER Testing Framework
cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# NOTE: `TEST_NAME` is expected to be a command line arg
project(${TEST_NAME})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${TEST_NAME})

# Add sources to executable
target_sources(${TEST_NAME} PRIVATE
    # Add user sources here
    ${C_SOURCES}
)

# Add include paths
target_include_directories(${TEST_NAME} PRIVATE
    # Add user defined include paths
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

