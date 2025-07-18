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

add_library(test_registry STATIC)
target_sources( test_registry
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_registry.c
)

target_include_directories(test_registry
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_registry
    unity
)