IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )

	SET( SRC ${SRC}

		${PWD}/Stdout.cpp
		${PWD}/Console.cpp

		PARENT_SCOPE )

ENDIF ()
