cmake_minimum_required(VERSION 3.10)

project(GameClient)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加项目的头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加项目的源文件
set(SOURCES
    src/base64.cpp
    src/json_serializer.cpp
    src/NetworkManager.cpp
    src/tools.cpp
    src/zmq_client.cpp
    main.cpp
)

# 创建可执行文件
add_executable(GameClient ${SOURCES})

# 指定链接库的位置
link_directories(${CMAKE_SOURCE_DIR}/libs)

# 链接所需的库
target_link_libraries(GameClient
    libs/libjsoncpp.dll
    libjsoncpp.so
    libs/libjsoncpp.so.1.9.5
    libs/libjsoncpp.so.25
    libzmq.so
    libopencv_core.so.4.5.4d
    libopencv_highgui.so.4.5.4d
    libopencv_imgcodecs.so.4.5.4d
    libopencv_imgproc.so.4.5.4d
)
