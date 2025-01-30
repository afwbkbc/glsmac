SUBDIR( context )
SUBDIR( type )
SUBDIR( callable )
SUBDIR( parser )
SUBDIR( program )
SUBDIR( runner )
SUBDIR( builtins )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
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
