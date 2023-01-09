SUBDIR( base )
SUBDIR( types )
SUBDIR( util )
SUBDIR( error_handler )
SUBDIR( logger )
SUBDIR( loader )
SUBDIR( renderer )
SUBDIR( scheduler )
SUBDIR( scene )
SUBDIR( procedural )
SUBDIR( task )
SUBDIR( ui )
SUBDIR( engine )

SUBDIR( opensmac )

SET(SRC ${SRC}

	${PWD}/main.cpp

PARENT_SCOPE)
