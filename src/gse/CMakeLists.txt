SUBDIR( context )
SUBDIR( value )
SUBDIR( callable )
SUBDIR( parser )
SUBDIR( program )
SUBDIR( runner )
SUBDIR( builtins )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "FastDebug" )
	SUBDIR( tests )
ENDIF ()

SET( SRC ${SRC}

	${PWD}/GSE.cpp
	${PWD}/Value.cpp
	${PWD}/ExecutionPointer.cpp
	${PWD}/Exception.cpp
	${PWD}/Wrappable.cpp
	${PWD}/Async.cpp

)

SET( SRC ${SRC} PARENT_SCOPE )
