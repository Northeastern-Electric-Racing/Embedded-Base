add_library(middleware_pid EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/pid.c
  )

target_include_directories(middleware_pid
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(middleware_pid PRIVATE stm32cubemx)