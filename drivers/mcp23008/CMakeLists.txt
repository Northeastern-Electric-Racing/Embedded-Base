add_library(driver_mcp23008 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/mcp23008.c
  )

target_include_directories(driver_mcp23008
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_mcp23008 PRIVATE stm32cubemx)