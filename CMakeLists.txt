cmake_minimum_required(VERSION 3.10)

project(GameClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES_DIR "src/*.cpp")

add_executable(GameClient main.cpp ${SOURCES_DIR})
target_include_directories(GameClient PRIVATE ${INCLUDE_DIR})

find_package(OpenCV REQUIRED)

set(ZMQ_LIBRARY_PATH "/usr/lib/x86_64-linux-gnu/libzmq.so")

target_link_libraries(GameClient
    ${ZMQ_LIBRARY_PATH}
    ${OpenCV_LIBS}
)

