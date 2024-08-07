add_library(driver_ltc68041 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/ltc68041.c
  )

target_include_directories(driver_ltc68041
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_ltc68041 PRIVATE stm32cubemx)