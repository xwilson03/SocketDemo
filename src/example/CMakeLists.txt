add_executable(client_example
  client_example.cpp
)

target_link_libraries(client_example
  PRIVATE
    client
)


add_executable(server_example
  server_example.cpp
)

target_link_libraries(server_example
  PRIVATE
    server
)


install(
  TARGETS
    client_example
    server_example
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)
