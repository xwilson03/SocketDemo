find_package(spdlog REQUIRED)

add_executable(server_example
  main.cpp
)

target_link_libraries(server_example
  PRIVATE
    server
    spdlog::spdlog
)

install(
  TARGETS
    server_example
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)
