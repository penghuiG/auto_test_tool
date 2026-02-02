cmake_minimum_required (VERSION 3.10)
 
project (tsu_test)

include_directories(. bsp bsp/can src common)
# ADDSOURCE_RECURSION(${CMAKE_CURRENT_SOURCE_DIR}/src)
# aux_source_directory(. SRC_LIST)

add_subdirectory(/home/cx/package/OpenXLSX ${CMAKE_BINARY_DIR}/OpenXLSX-build)
add_subdirectory(http_server)

aux_source_directory(bsp/can SRC_LIST1)
aux_source_directory(bsp SRC_LIST2)
aux_source_directory(src SRC_LIST3)
aux_source_directory(tools SRC_LIST4)
aux_source_directory(common SRC_LIST5)

add_executable(${PROJECT_NAME} ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3} ${SRC_LIST4} ${SRC_LIST5})


target_link_libraries(${PROJECT_NAME}
    -lpthread 
    -lwiringPi
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/can/libcontrolcan.so
    OpenXLSX
    )

