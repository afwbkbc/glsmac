SUBDIR( object )
SUBDIR( element )
SUBDIR( theme )
SUBDIR( event )

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	SUBDIR( debug )
ENDIF()

SET(SRC ${SRC}

	${PWD}/UI.cpp

PARENT_SCOPE)
