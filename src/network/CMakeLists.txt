set (SOURCES
	Connection.cpp
	ConnectionManager.cpp
	NetworkMessage.cpp
	OutputMessage.cpp
	Protocol.cpp
	ServiceManager.cpp
	ServicePort.cpp
	)
source_group(Network FILES ${SOURCES})
set(PROTOCOL_SOURCES
	protocol/Telnet.cpp
	)
source_group(Network\\Protocol FILES ${PROTOCOL_SOURCES})
list(APPEND SOURCES
	${PROTOCOL_SOURCES}
	)

add_library(Network ${SOURCES})
add_dependencies(Network
	CommonEnumsGens
	)
set_target_properties(Network PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	)
target_link_libraries(Network
	PRIVATE
		Lotospp-buildinfo
		CommonEnums
		Boost::thread
	)
