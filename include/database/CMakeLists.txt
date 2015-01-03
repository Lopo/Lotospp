set(lotos2_HEADERS
	Driver.h
	Insert.h
	Query.h
	Result.h
	Transaction.h
	)

install(FILES ${lotos2_HEADERS} DESTINATION "${lotos2_INCLUDE_DIR}/lotos2/database")

add_subdirectory(driver)
