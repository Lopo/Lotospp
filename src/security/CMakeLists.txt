set (SOURCES
	Blowfish.cpp
	)
source_group(Security FILES ${SOURCES})

add_library(Security ${SOURCES})
set_target_properties(Security PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
#add_dependencies(Security
#	SecurityEnums
#	)
#target_link_libraries(Command PRIVATE Lotospp-buildinfo generated)
#target_link_libraries(Security PRIVATE generated)
#target_link_libraries(Security PRIVATE SecurityEnums)
#target_link_libraries(Security PRIVATE Boost::random)

#target_include_directories(Security PRIVATE ${ARGON2_DIR}/include)
#target_link_directories(Security PRIVATE ${ARGON2_DIR})
#target_link_libraries(Security PRIVATE argon2)
