find_package(spdlog REQUIRED)


add_executable(client_example
  main.cpp
)

target_link_libraries(client_example
  PRIVATE
    client
    spdlog::spdlog
)

install(
  TARGETS
    client_example
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)
