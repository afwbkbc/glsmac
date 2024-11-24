SUBDIR( common )
SUBDIR( intro )
SUBDIR( mainmenu )
SUBDIR( game )
SUBDIR( console )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SUBDIR( gseprompt )
	SUBDIR( gsetests )
ENDIF ()

SET( SRC ${SRC}

	PARENT_SCOPE )
