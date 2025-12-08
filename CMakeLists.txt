cmake_minimum_required(VERSION 3.10)
project(pcf8563 VERSION 0.5.0 LANGUAGES C)

if (ESP_PLATFORM)

    idf_component_register(
        SRCS "pcf8563.c"
        INCLUDE_DIRS "."
    )

else()

    add_library(pcf8563 pcf8563.c)
    target_include_directories(pcf8563 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    # Tests (off by default)
    option(PCF8563_BUILD_TESTS "Build tests" OFF)
    if (PCF8563_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()

endif()
