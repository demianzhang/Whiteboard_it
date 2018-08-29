cmake_minimum_required(VERSION 3.7)
project(hw)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp hough.cpp)
add_executable(hw ${SOURCE_FILES})
target_link_libraries( hw ${OpenCV_LIBS})