cmake_minimum_required(VERSION 3.10)
project(https_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找OpenSSL
find_package(OpenSSL REQUIRED)

# 添加可执行文件
# add_executable(https_server src/http_main.cpp)
aux_source_directory(src SRC_LIST)
add_executable(https_server ${SRC_LIST})
# 链接所有需要的库
target_link_libraries(${PROJECT_NAME}
    pthread
    ${OPENSSL_LIBRARIES}
    wiringPi
    OpenXLSX
)

# 设置编译选项
target_compile_options(https_server PRIVATE -Wall -Wextra)