cmake_minimum_required(VERSION 3.10)

project(GameClient)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/base64.cpp
    main.cpp
)

add_executable(GameClient-Demo ${SOURCES})

link_directories(${CMAKE_SOURCE_DIR}/libs)
target_link_libraries(GameClient
    ${CMAKE_SOURCE_DIR}/libs/libjsoncpp.so
    ${CMAKE_SOURCE_DIR}/libs/libzmq.so
    ${CMAKE_SOURCE_DIR}/libs/libopencv_core.so.4.5.4d
    ${CMAKE_SOURCE_DIR}/libs/libopencv_highgui.so.4.5.4d
    ${CMAKE_SOURCE_DIR}/libs/libopencv_imgcodecs.so.4.5.4d
    ${CMAKE_SOURCE_DIR}/libs/libopencv_imgproc.so.4.5.4d
)
