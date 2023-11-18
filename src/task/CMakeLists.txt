SUBDIR( common )
SUBDIR( intro )
SUBDIR( mainmenu )
SUBDIR( game )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SUBDIR( tests )
ENDIF ()

SET( SRC ${SRC}

	PARENT_SCOPE )
