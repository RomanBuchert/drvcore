
# Test registry.c
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
    driver
    unity
)

# Test driver.c
add_library(test_driver STATIC)
target_sources( test_driver
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_driver.c
)
target_include_directories(test_driver
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_driver
    driver
    unity
)