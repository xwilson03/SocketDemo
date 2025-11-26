find_package(spdlog REQUIRED)

add_library(server
  server.cpp
)

target_link_libraries(server
  PUBLIC
    ev::ev
    spdlog::spdlog
)

target_include_directories(server
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
