cmake_minimum_required(VERSION 2.8.4)
project(ObjectTracker)



set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -std=c++11")

set(SOURCE_FILES
    include/Position.hh
    include/Tracker.hh
    src/main.cpp
    src/Tracker.cpp
    )

include_directories(include)

add_executable(ObjectTracker ${SOURCE_FILES})

target_link_libraries (ObjectTracker opencv_core opencv_highgui opencv_imgproc)

add_definitions(-std=c++11)
