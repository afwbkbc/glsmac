SUBDIR( type )
SUBDIR( runnable )
SUBDIR( builtin )

SET( SRC ${SRC}

	${PWD}/GSE.cpp
	${PWD}/Value.cpp
	${PWD}/Exception.cpp

	)

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SET( SRC ${SRC}

		${PWD}/Tests.cpp

		)
ENDIF ()

SET( SRC ${SRC} PARENT_SCOPE )
