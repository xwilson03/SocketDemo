add_executable(server_example
  main.cpp
)

target_link_libraries(server_example
  PRIVATE
    server
)

install(
  TARGETS
    server_example
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)
