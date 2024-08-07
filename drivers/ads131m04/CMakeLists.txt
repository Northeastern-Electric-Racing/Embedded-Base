add_library(driver_ads131m04 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/ads131m04.c
  )

target_include_directories(driver_ads131m04
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_ads131m04 PRIVATE stm32cubemx)

  