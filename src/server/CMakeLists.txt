add_executable(server
  server.cpp
)

install(
  TARGETS server
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
