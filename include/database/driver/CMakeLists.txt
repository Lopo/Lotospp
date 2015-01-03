if (MYSQL_FOUND)
	set(lotos2_HEADERS
		MySQL.h
		)
endif (MYSQL_FOUND)

install(FILES ${lotos2_HEADERS} DESTINATION "${lotos2_INCLUDE_DIR}/lotos2/database/driver")
