add_library(middleware_ringbuffer EXCLUDE_FROM_ALL
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/ringbuffer.c
  )

target_include_directories(middleware_ringbuffer
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )