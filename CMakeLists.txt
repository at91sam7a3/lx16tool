cmake_minimum_required(VERSION 2.9)

project(lx16tool)

add_subdirectory("${CMAKE_SOURCE_DIR}/../lx16lib" "${CMAKE_SOURCE_DIR}/../lx16lib/build")

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(lx16tool lx16lib)