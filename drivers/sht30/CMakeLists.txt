add_library(driver_sht30 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/sht30.c
  )

target_include_directories(driver_sht30
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_sht30 PRIVATE stm32cubemx)