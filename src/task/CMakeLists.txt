SUBDIR( common )
SUBDIR( main )
SUBDIR( intro )
SUBDIR( mainmenu )
SUBDIR( game )
SUBDIR( console )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "FastDebug" )
	SUBDIR( gseprompt )
	SUBDIR( gsetests )
ENDIF ()

SET( SRC ${SRC}

	PARENT_SCOPE )
