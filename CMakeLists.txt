cmake_minimum_required (VERSION 3.10)
 
project (tsu_test)

include_directories(. bsp bsp/can src common services)
# ADDSOURCE_RECURSION(${CMAKE_CURRENT_SOURCE_DIR}/src)
# aux_source_directory(. SRC_LIST)

add_subdirectory(/home/cx/package/OpenXLSX ${CMAKE_BINARY_DIR}/OpenXLSX-build)
add_subdirectory(http_server)



aux_source_directory(src SRC_LIST_SRC)
aux_source_directory(bsp SRC_LIST_BSP)
aux_source_directory(tools SRC_LIST_TOOLS)
aux_source_directory(common SRC_LIST_COMMON)
aux_source_directory(bsp/can SRC_LIST_CAN)
aux_source_directory(services SRC_LIST_SERVICES)

add_executable(${PROJECT_NAME} ${SRC_LIST_SRC} ${SRC_LIST_BSP} ${SRC_LIST_TOOLS} ${SRC_LIST_COMMON} ${SRC_LIST_CAN} ${SRC_LIST_SERVICES})


target_link_libraries(${PROJECT_NAME}
    -lpthread 
    -lwiringPi
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/can/libcontrolcan.so
    OpenXLSX
    )

