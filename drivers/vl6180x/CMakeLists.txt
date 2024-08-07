add_library(driver_vl6180x EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/vl6180x_api.c
    ${CMAKE_CURRENT_LIST_DIR}/vl6180x_i2c.c
  )

target_include_directories(driver_vl6180x
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_vl6180x PRIVATE stm32cubemx)