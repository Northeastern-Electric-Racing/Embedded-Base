cmake_minimum_required(VERSION 3.22)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/nanopb/extra)
find_package(Nanopb REQUIRED)

nanopb_generate_cpp(TARGET proto serverdata.proto)

target_link_libraries(${CMAKE_PROJECT_NAME} proto)
