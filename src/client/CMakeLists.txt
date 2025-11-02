add_executable(client
  client.cpp
)

install(
  TARGETS client
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
