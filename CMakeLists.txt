cmake_minimum_required(VERSION 3.25)

project("Generic Driver")

add_library(drv_core STATIC
    ${CMAKE_SOURCE_DIR}/src/driver.c
    ${CMAKE_SOURCE_DIR}/src/registry.c
    ${CMAKE_SOURCE_DIR}/src/drv_core.c
)

target_include_directories(drv_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

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
    drv_core
    unity
)

add_test(
    NAME core_test
    COMMAND drv_core_test
)