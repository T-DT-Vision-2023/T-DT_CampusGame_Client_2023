cmake_minimum_required(VERSION 3.10)

project(GameClient)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加项目的头文件目录
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# 自动收集 src 目录下的所有源文件
file(GLOB SOURCES_DIR "src/*.cpp")

# 创建可执行文件
add_executable(GameClient ${SOURCES_DIR})

# 设置头文件目录
target_include_directories(GameClient PRIVATE ${INCLUDE_DIR})

# 寻找所需的库
find_package(jsoncpp REQUIRED)
find_package(OpenCV REQUIRED)

set(ZMQ_LIBRARY_PATH "/usr/lib/x86_64-linux-gnu/libzmq.so")

target_link_libraries(GameClient
    jsoncpp_lib
    ${ZMQ_LIBRARY_PATH}
    ${OpenCV_LIBS}
)

