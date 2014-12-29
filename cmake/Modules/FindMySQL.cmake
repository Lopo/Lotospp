# - Find mysql client library and includes
#
#  MYSQL_FOUND            - True if mysql was found
#  MYSQL_INCLUDE_DIRS     - the include dir where mysql.h lives
#  MYSQL_LIBRARIES        - list of mysql libraries

if (MYSQL_LIBRARIES AND MYSQL_INCLUDE_DIRS)
	# in cache already, be silent and skip the rest
	set(MYSQL_FOUND TRUE)
else ()
	# Find the include dir:
	FIND_PATH(MYSQL_INCLUDE_DIRS mysql.h
		/usr/include/mysql
		/usr/local/include/mysql
		/opt/mysql/mysql/include
		/opt/mysql/mysql/include/mysql
		/usr/local/mysql/include
		/usr/local/mysql/include/mysql
		$ENV{ProgramFiles}/MySQL/*/include
		$ENV{SystemDrive}/MySQL/*/include
		)

	# Find the library:
	SET(MYSQL_LIBRARY_NAMES mysqlclient mysqlclient_r)
	FIND_LIBRARY(MYSQL_LIBRARIES
		NAMES ${MYSQL_LIBRARY_NAMES}
		PATHS /usr/lib
		/usr/local/lib
		/usr/local/mysql/lib
		/opt/mysql/mysql/lib
		$ENV{ProgramFiles}/MySQL/*/lib
		PATH_SUFFIXES mysql
		)
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(MySQL  DEFAULT_MSG
		MYSQL_INCLUDE_DIRS MYSQL_LIBRARIES
		)
	mark_as_advanced(MYSQL_INCLUDE_DIRS MYSQL_LIBRARIES)
endif () #(MYSQL_LIBRARIES AND MYSQL_INCLUDE_DIRS)
