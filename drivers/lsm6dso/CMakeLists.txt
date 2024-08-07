add_library(driver_lsm6dso EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/lsm6dso.c
  )

target_include_directories(driver_lsm6dso
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_lsm6dso PRIVATE stm32cubemx)