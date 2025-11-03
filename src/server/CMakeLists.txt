include(FetchContent)
FetchContent_Declare(ev
  GIT_REPOSITORY https://github.com/sjinks/libev-cmake
  GIT_TAG v4.33
)
FetchContent_MakeAvailable(ev)

add_executable(server
  server.cpp
)

target_link_libraries(server
  ev::ev
)

install(
  TARGETS server
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
