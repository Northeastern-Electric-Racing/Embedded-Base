add_library(middleware_eepromdirectory EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/eepromdirectory.c
  )

target_include_directories(middleware_eepromdirectory
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

target_link_libraries(middleware_eepromdirectory PRIVATE embedded_base_general stm32cubemx)