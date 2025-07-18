cmake_minimum_required(VERSION 3.25)

project(drv_core)

add_library(drv_core STATIC)

target_sources( drv_core
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/drv_core.c
)

target_include_directories( drv_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_link_libraries( drv_core
    driver

)