macro(make_small_executable targetName)
	add_custom_command(TARGET ${targetName}
		POST_BUILD 
		COMMAND ${CMAKE_COMMAND} 
			-D EXECUTE_POST_BUILD=$<CONFIG:MinSizeRel>
			-D TARGET_FILE="$<TARGET_FILE:${targetName}>"
			-D CMAKE_STRIP="${CMAKE_STRIP}"
			-P ${CMAKE_SOURCE_DIR}/cmake/ReallySmallPostBuild.cmake
		)
endmacro()
