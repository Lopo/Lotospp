file(GLOB_RECURSE SRC_LIST *.cpp)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(Lotos2
	${Boost_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${MYSQL_LIBRARIES}
	)

# Must export symbols from daemon so that plugins can use them
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_EXPORTS_CXX_FLAG}")

# To find public headers
include_directories(
	BEFORE
	${PROJECT_SOURCE_DIR}/include
	${Boost_INCLUDE_DIR}
	${MYSQL_INCLUDE_DIR}
	)
# To find config.h & version.h
include_directories(
	BEFORE
	${PROJECT_BINARY_DIR}/include
	)

# To find local headers
#include(${CMAKE_CURRENT_SOURCE_DIR})

#include_directories()


#target_link_libraries(Lotos2 ${Boost_LIBRARIES})

#install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/Lotos2 DESTINATION ${LP_INSTALL_BIN_DIR})

#add_subdirectory(command)

#add_definitions(-DBOOST_ALL_DYN_LINK)
#find_package(Boost REQUIRED COMPONENTS program_options system filesystem log log_setup thread)
#message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
#message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
#message(status "** Boost Libraries: ${Boost_LIBRARIES}")
#set(CMAKE_THREAD_PREFER_PTHREAD 1)
#find_package(Threads REQUIRED)
#message(status "")

link_directories(${Boost_LIBRARY_DIRS})

#install(TARGETS Lotos2 RUNTIME DESTINATION bin)




#include_directories(${CMAKEDEMO_SOURCE_DIR}/include)
#link_directories(${CMAKEDEMO_BINARY_DIR}/bin)

#the one C file
#add_executable(Lotos+ main.c)
#target_link_libraries(Lotos+ m) #link the math library

#these are all compiled the same way
#set(PROGRAMS oglfirst pointers)
#set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} m)

#foreach(program ${PROGRAMS})
#  add_executable(${program} ${program}.cpp)
#  target_link_libraries(${program} ${CORELIBS})
#endforeach(program)

#building just a library. 
#add_library(geometry geometry.cpp)
