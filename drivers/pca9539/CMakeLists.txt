add_library(driver_pca9539 EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/pca9539.c
  )

target_include_directories(driver_pca9539
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port_header
  )

  target_link_libraries(driver_pca9539 PRIVATE stm32cubemx)