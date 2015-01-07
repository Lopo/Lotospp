set(lotos2_HEADERS
	Connection.h
	NetworkMessage.h
	OutputMessage.h
	Protocol.h
	ProtocolConst.h
	ServiceBase.h
	Service.h
	ServicePort.h
	ServiceManager.h
	)

install(FILES ${lotos2_HEADERS} DESTINATION "${lotos2_INCLUDE_DIR}/lotos2/network")

add_subdirectory(protocol)
