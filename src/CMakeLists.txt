add_executable(client
  client.cpp
)

add_executable(server
  server.cpp
)

install(
  TARGETS
    client
    server
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)
