add_library(driver_max7314 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/max7314.c
  )

target_include_directories(driver_max7314
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_max7314 PRIVATE stm32cubemx)