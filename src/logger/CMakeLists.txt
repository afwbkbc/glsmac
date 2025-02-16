IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "FastDebug" )

	SET( SRC ${SRC}

		${PWD}/Stdout.cpp
		${PWD}/Console.cpp

		PARENT_SCOPE )

ENDIF ()
