cmake_minimum_required(VERSION 3.25)

add_library(driver STATIC)

target_sources( driver
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/driver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/registry.c
        ${CMAKE_CURRENT_SOURCE_DIR}/properties.c
)

target_include_directories( driver
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

add_subdirectory(tests)