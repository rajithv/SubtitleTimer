cmake_minimum_required(VERSION 2.8)
project(SubtitleTimer)

find_package( OpenCV REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(SubtitleTimer.o ${SOURCE_FILES})

target_link_libraries( SubtitleTimer.o ${OpenCV_LIBS} tesseract )
