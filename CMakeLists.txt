cmake_minimum_required(VERSION 3.16)
project(ks-psp VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(KS_BUILD_APP "Build the SDL2 application" ON)
option(KS_BUILD_TESTS "Build the host unit tests" ON)

set(KS_WARNINGS -Wall -Wextra -Wshadow -Wstrict-prototypes -Wmissing-prototypes)

# Engine core: portable C99, no SDL, no platform headers. Everything here is unit tested on the host.
add_library(kscore STATIC
    src/crc32.c
)
target_include_directories(kscore PUBLIC src)
target_compile_options(kscore PRIVATE ${KS_WARNINGS})

if(KS_BUILD_APP)
    find_package(SDL2 REQUIRED)
    add_executable(ks src/main.c)
    target_include_directories(ks PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(ks PRIVATE kscore ${SDL2_LIBRARIES})
    target_compile_options(ks PRIVATE ${KS_WARNINGS})

    if(PSP)
        create_pbp_file(
            TARGET ks
            ICON_PATH NULL
            BACKGROUND_PATH NULL
            PREVIEW_PATH NULL
            TITLE "Katawa Shoujo"
            VERSION 01.00
        )
    endif()
endif()

if(KS_BUILD_TESTS AND NOT PSP)
    enable_testing()
    # One executable per file in tests/c named test_<module>.c
    file(GLOB KS_TEST_SOURCES CONFIGURE_DEPENDS tests/c/test_*.c)
    foreach(test_source ${KS_TEST_SOURCES})
        get_filename_component(test_name ${test_source} NAME_WE)
        add_executable(${test_name} ${test_source})
        target_include_directories(${test_name} PRIVATE tests/c)
        target_link_libraries(${test_name} PRIVATE kscore)
        target_compile_options(${test_name} PRIVATE ${KS_WARNINGS})
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()
