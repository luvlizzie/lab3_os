cmake_minimum_required(VERSION 3.14)
project(ThreadsLab3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исполняемый файл
add_executable(markers.exe 
    src/main.cpp
    src/marker.cpp
    src/utils.cpp
)

target_include_directories(markers.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Поддержка Windows
if (MINGW)
    set_target_properties(markers.exe PROPERTIES SUFFIX "")
endif()

# Поддержка потоков
find_package(Threads REQUIRED)
target_link_libraries(markers.exe PRIVATE Threads::Threads)

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Тестовый раннер
add_executable(test_runner 
    tests/test_runner.cpp
    tests/test_marker.cpp
    src/utils.cpp
)

target_link_libraries(test_runner 
    gtest_main
    gmock
    pthread
)

target_include_directories(test_runner PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

enable_testing()
add_test(NAME all_tests COMMAND test_runner)
