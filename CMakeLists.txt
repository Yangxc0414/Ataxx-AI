cmake_minimum_required(VERSION 3.10)
project(Ataxx-AI)

# 设置C++标准
s et(CMAKE_CXX_STANDARD 11)
s et(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
file(GLOB SOURCES "*.cpp")

# 创建可执行文件
add_executable(ataxx ${SOURCES})

# 如果在Windows系统上，需要链接ws2_32库以支持套接字功能
if(WIN32)
    target_link_libraries(ataxx ws2_32)
endif()