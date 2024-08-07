add_library(middleware_c_utils EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/c_utils.c
  )

target_include_directories(middleware_c_utils
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )