add_library(embedded_base_general
  STATIC
    #${CMAKE_CURRENT_LIST_DIR}/src/ads131m04.c
    #${CMAKE_CURRENT_LIST_DIR}/src/lsm6dso.c
    #${CMAKE_CURRENT_LIST_DIR}/src/LTC4015.c
    #${CMAKE_CURRENT_LIST_DIR}/src/ltc68041.c
    ${CMAKE_CURRENT_LIST_DIR}/src/m24c32.c
    ${CMAKE_CURRENT_LIST_DIR}/src/max7314.c
    #${CMAKE_CURRENT_LIST_DIR}/src/mcp23008.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pca9539.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pi4ioe.c
    ${CMAKE_CURRENT_LIST_DIR}/src/sht30.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vl6180x_api.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vl6180x_i2c.c
  )

target_include_directories(embedded_base_general
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
  )
target_link_libraries(embedded_base_general PRIVATE stm32cubemx)