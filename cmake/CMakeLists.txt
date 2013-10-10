set(Lotos2_CMAKE
	cmake_uninstall.cmake.in
#	FindLicq.cmake
	CommonCompilerFlags.cmake
	${CMAKE_CURRENT_BINARY_DIR}/Config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/Paths.cmake
	)

configure_file(
	Config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/Config.cmake
	@ONLY
	)

configure_file(
	Paths.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/Paths.cmake
	@ONLY
	)

install(FILES ${Lotos2_CMAKE} DESTINATION ${Lotos2_SHARE_DIR}/cmake)

#add_subdirectory(Modules)
