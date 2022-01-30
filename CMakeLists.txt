cmake_minimum_required(VERSION 3.15)

project(scheduler)

find_package (GTest REQUIRED)
find_package (Threads REQUIRED)

add_executable(dummy_test tst.cpp)

set_target_properties(dummy_test
        PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSION OFF
)

target_compile_options(dummy_test
        PRIVATE
        "-Wall;-Wextra;-Wpedantic;-Werror"
        )

target_link_libraries(dummy_test
        PRIVATE
        "Threads::Threads;${GTEST_LIBRARY};${GTEST_MAIN_LIBRARY};"
        )

target_include_directories(dummy_test
        PRIVATE
        "${CMAKE_SOURCE_DIR}"
        )

add_test(NAME test-scheduler
        COMMAND dummy_test
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )

include(CTest)
