add_library(driver_pi4ioe EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/pi4ioe.c
  )

target_include_directories(driver_pi4ioe
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_pi4ioe PRIVATE stm32cubemx)