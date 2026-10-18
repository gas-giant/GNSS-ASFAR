cmake_minimum_required(VERSION 3.24)
project(GNSS_ASFAR LANGUAGES CXX)

# simulator
add_library(simulator 
    src/simulator/acquisition/fft.cpp 
    src/simulator/fixedPoint.cpp
    src/main.cpp)
target_include_directories(simulator PUBLIC 
    include)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)

    add_executable(my_tests 
        tests/fixedPointTest.cpp
        tests/complexTest.cpp
        )
    target_include_directories(my_tests 
        PRIVATE
            include)
    target_link_libraries(my_tests 
        PRIVATE 
            GTest::gmock_main
            simulator)

    include(GoogleTest)
    gtest_discover_tests(my_tests)
endif()