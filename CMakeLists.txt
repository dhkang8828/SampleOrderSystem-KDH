cmake_minimum_required(VERSION 3.20)
project(SampleOrderSystem VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test (FetchContent)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# sqlite3 amalgamation (third_party/)
add_library(sqlite3 STATIC third_party/sqlite3.c)
target_include_directories(sqlite3 PUBLIC third_party/)

# 소스 파일 수집
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/**/*.cpp")

# 메인 실행파일
add_executable(SampleOrderSystem main.cpp ${SOURCES})
target_link_libraries(SampleOrderSystem PRIVATE sqlite3)
target_include_directories(SampleOrderSystem PRIVATE src/ third_party/)

# 테스트 실행파일
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp" "tests/**/*.cpp")
add_executable(run_tests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(run_tests PRIVATE
    sqlite3
    GTest::gtest
    GTest::gtest_main
)
target_include_directories(run_tests PRIVATE src/ third_party/)

enable_testing()
add_test(NAME AllTests COMMAND run_tests)
