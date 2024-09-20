cmake_minimum_required(VERSION 3.28.3)
project(server)

include_directories(.)

add_library(server_lib server.cpp)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} server_lib)
