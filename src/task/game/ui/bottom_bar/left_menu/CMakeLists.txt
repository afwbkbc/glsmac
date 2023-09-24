SET( SRC ${SRC}

	${PWD}/LeftMenu.cpp
	${PWD}/GameMenu.cpp
	${PWD}/EditMapMenu.cpp

	)

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SET( SRC ${SRC}

		${PWD}/DebugMenu.cpp

		)
ENDIF ()

SET( SRC ${SRC} PARENT_SCOPE )
