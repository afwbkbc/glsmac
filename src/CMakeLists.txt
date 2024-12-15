SUBDIR( common )
SUBDIR( types )
SUBDIR( util )
SUBDIR( config )
SUBDIR( error_handler )
SUBDIR( game )
SUBDIR( gse )
SUBDIR( logger )
SUBDIR( resource )
SUBDIR( loader )
SUBDIR( input )
SUBDIR( graphics )
SUBDIR( audio )
SUBDIR( network )
SUBDIR( scheduler )
SUBDIR( scene )
SUBDIR( ui_legacy )
SUBDIR( engine )
SUBDIR( rr )
SUBDIR( task )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SUBDIR( debug )
ENDIF ()

SET( SRC ${SRC}

	${PWD}/main.cpp

	PARENT_SCOPE )
