add_library(middleware_timer EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/timer.c
  )

target_include_directories(middleware_timer
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(middleware_timer PRIVATE stm32cubemx)