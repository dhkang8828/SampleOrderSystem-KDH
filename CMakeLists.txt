cmake_minimum_required(VERSION 3.20)
project(SampleOrderSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/**/*.cpp")

add_executable(SampleOrderSystem main.cpp ${SOURCES})
target_link_libraries(SampleOrderSystem PRIVATE unofficial::sqlite3::sqlite3)

file(GLOB_RECURSE TEST_SOURCES "tests/**/*.cpp")
add_executable(run_tests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(run_tests PRIVATE
    unofficial::sqlite3::sqlite3
    GTest::gtest
    GTest::gtest_main
)

enable_testing()
add_test(NAME AllTests COMMAND run_tests)
