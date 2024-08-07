add_library(embedded_base_middleware
  STATIC
    ${CMAKE_CURRENT_LIST_DIR}/src/c_utils.c
    #${CMAKE_CURRENT_LIST_DIR}/src/eepromdirectory.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pid.c
    ${CMAKE_CURRENT_LIST_DIR}/src/ringbuffer.c
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.c
  )

target_include_directories(embedded_base_middleware
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

target_link_libraries(embedded_base_middleware stm32cubemx)