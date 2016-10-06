cmake_minimum_required(VERSION 3.6)
project(shortest-path-project1)

#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall Werror")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${-L/usr/X11R6/lib} ${XLIB_LIB_COMPILE_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_STANDARD 11)


find_package(X11)
link_libraries(${X11_LIBRARIES})

#link_directories(/usr/X11R6/lib)

#include_directories(/usr/share/X11)
include_directories(${X11_INCLUDE_DIR})

#g++ main.cpp -L/usr/X11R6/lib -lX11

#include_directories(${-L/usr/X11R6/lib})


set(SOURCE_FILES main.cpp)
add_executable(shortest-path-project1 ${SOURCE_FILES})