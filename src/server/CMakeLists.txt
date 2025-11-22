include(FetchContent)


FetchContent_Declare(ev
  GIT_REPOSITORY https://github.com/sjinks/libev-cmake
  GIT_TAG v4.33
)

FetchContent_MakeAvailable(ev)


add_library(server
  server.cpp
)

target_link_libraries(server
  PUBLIC
    ev::ev
)

target_include_directories(server
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
