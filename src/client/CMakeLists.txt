cmake_minimum_required(VERSION 3.28.3)
project(client)

include_directories(.)

add_library(client_lib client.cpp)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} client_lib)
