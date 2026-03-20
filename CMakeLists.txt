cmake_minimum_required (VERSION 3.10)
 
project (version_base_test)



add_subdirectory(third_library/OpenXLSX)

add_subdirectory(http_server)

aux_source_directory(bsp SRC_LIST)
aux_source_directory(src SRC_LIST)
aux_source_directory(tools SRC_LIST)
aux_source_directory(common SRC_LIST)
aux_source_directory(services SRC_LIST)
add_executable(${PROJECT_NAME}   ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PRIVATE
    .
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

