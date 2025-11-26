find_package(spdlog REQUIRED)

add_executable(stress_test
  main.cpp
)

target_link_libraries(stress_test
  PRIVATE
    client
    server
    spdlog::spdlog
)

install(
  TARGETS
    stress_test
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)
