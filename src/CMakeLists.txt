add_executable(main
  main.cpp
)
install(
  TARGETS
    main
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)