cmake_minimum_required(VERSION 3.10)

project(GameClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES_DIR "src/*.cpp")

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/include/network)
include_directories(${PROJECT_SOURCE_DIR}/include/core)

add_executable(GameClient main.cpp ${SOURCES_DIR})


find_package(OpenCV REQUIRED)
find_library(ZMQ_LIBRARY NAMES zmq)

target_link_libraries(GameClient
        ${ZMQ_LIBRARY}
        ${OpenCV_LIBS}
)
