cmake_minimum_required (VERSION 3.10)
 
project (auto_test_lib)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


add_subdirectory(third_library/OpenXLSX)

add_subdirectory(http_server)

set(SRC_LIST "")
aux_source_directory(bsp SRC_LIST)
aux_source_directory(src SRC_LIST)
aux_source_directory(tools SRC_LIST)
aux_source_directory(common SRC_LIST)
aux_source_directory(services SRC_LIST)

add_library(${PROJECT_NAME}   SHARED ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    bsp
    bsp/can
    common
    services
    src
)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    wiringPi
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/can/libcontrolcan.so
    OpenXLSX
)

add_executable(low_power_test ${CMAKE_CURRENT_SOURCE_DIR}/sample/low_power_test.cpp)

target_link_libraries(low_power_test
    ${PROJECT_NAME}
)

add_executable(emmc_test ${CMAKE_CURRENT_SOURCE_DIR}/sample/emmc_test.cpp)

find_package(Threads REQUIRED)
target_link_libraries(emmc_test
    ${PROJECT_NAME}
)