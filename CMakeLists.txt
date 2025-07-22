cmake_minimum_required(VERSION 3.25)

project("Generic Driver")

# Subprojects
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/driver)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/drv_core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/drv_dio)

# Testing
include(FetchContent)

FetchContent_Declare(
  Unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG        master   # Oder z. B. v2.5.2
)

# Inhalt abrufen
FetchContent_MakeAvailable(Unity)

enable_testing()

add_executable(drv_core_test
    ${CMAKE_SOURCE_DIR}/test/test01.c
)

target_include_directories(drv_core_test
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(drv_core_test
    driver
    drv_core
    drv_dio
    unity
)

add_test(
    NAME core_test
    COMMAND drv_core_test
)

target_link_libraries(drv_core_test
    test_registry
    test_driver

)