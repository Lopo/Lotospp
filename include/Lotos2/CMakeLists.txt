set(Lotos2_HEADERS
	${CMAKE_CURRENT_BINARY_DIR}/version.h

	version.h
)

install(FILES ${Lotos2_HEADERS} DESTINATION "${Lotos2_INCLUDE_DIR}/Lotos2")

add_subdirectory(exceptions)
