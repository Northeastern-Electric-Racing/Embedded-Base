add_library(driver_ltc4015 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/LTC4015.c
  )

target_include_directories(driver_ltc4015
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_ltc4015 PRIVATE stm32cubemx)