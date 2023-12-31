SUBDIR( type )
SUBDIR( callable )
SUBDIR( parser )
SUBDIR( runner )
SUBDIR( builtin )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SUBDIR( tests )
ENDIF ()

SET( SRC ${SRC}

	${PWD}/GSE.cpp
	${PWD}/Value.cpp
	${PWD}/Context.cpp

	)

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SET( SRC ${SRC}

		${PWD}/Tests.cpp

		)
ENDIF ()

SET( SRC ${SRC} PARENT_SCOPE )
