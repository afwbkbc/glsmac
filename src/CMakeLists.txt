SUBDIR( base )
SUBDIR( types )
SUBDIR( util )
SUBDIR( config )
SUBDIR( error_handler )
SUBDIR( logger )
SUBDIR( loader )
SUBDIR( input )
SUBDIR( graphics )
SUBDIR( audio )
SUBDIR( network )
SUBDIR( scheduler )
SUBDIR( scene )
SUBDIR( ui )
SUBDIR( engine )
SUBDIR( rr )
SUBDIR( game )

IF(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "MemoryDebug" OR CMAKE_BUILD_TYPE STREQUAL "Devel")
	SUBDIR( debug )
ENDIF()

SET(SRC ${SRC}

	${PWD}/main.cpp

PARENT_SCOPE)
