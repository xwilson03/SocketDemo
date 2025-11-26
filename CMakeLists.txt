cmake_minimum_required(VERSION 3.21)
project(socket_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

add_subdirectory(subs)
add_subdirectory(lib)
add_subdirectory(app)
