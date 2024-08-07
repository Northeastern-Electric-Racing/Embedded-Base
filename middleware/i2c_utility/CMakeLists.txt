add_library(middleware_i2c_utility EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/i2c_utility.c
  )

target_include_directories(middleware_i2c_utility
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

target_link_libraries(middleware_i2c_utility PRIVATE stm32cubemx)