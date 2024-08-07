add_library(driver_m24c32 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/m24c32.c
  )

target_include_directories(driver_m24c32
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_m24c32 PRIVATE stm32cubemx)