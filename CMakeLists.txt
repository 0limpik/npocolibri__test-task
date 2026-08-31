cmake_minimum_required(VERSION 3.19)
project(npocolibri__test-task LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Network)

qt_standard_project_setup()

add_subdirectory(shared)
add_subdirectory(client)
add_subdirectory(server)
