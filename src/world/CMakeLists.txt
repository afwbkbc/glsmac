SUBDIR( runnable )

SET( SRC ${SRC}

	${PWD}/World.cpp
	${PWD}/Exceptions.cpp

	)

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SET( SRC ${SRC}

		${PWD}/Tests.cpp

		)
ENDIF ()

SET( SRC ${SRC} PARENT_SCOPE )