cmake_minimum_required(VERSION 3.25)

project(drv_dio)

add_library(drv_dio STATIC)

target_sources( drv_dio
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_dio.c
)

target_include_directories( drv_dio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries( drv_dio
    driver
)