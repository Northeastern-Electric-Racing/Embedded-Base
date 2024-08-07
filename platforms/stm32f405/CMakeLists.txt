
add_library(embedded_base_platform EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/can.c
  )

target_include_directories(embedded_base_platform
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  )
target_link_libraries(embedded_base_platform PRIVATE middleware_c_utils stm32cubemx)