SUBDIR( type )
SUBDIR( callable )
SUBDIR( parser )
SUBDIR( runner )
SUBDIR( builtins )

IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	SUBDIR( tests )
ENDIF ()

SET( SRC ${SRC}

	${PWD}/GSE.cpp
	${PWD}/Value.cpp
	${PWD}/Context.cpp
	${PWD}/ChildContext.cpp
	${PWD}/GlobalContext.cpp
	${PWD}/Exception.cpp
	${PWD}/Wrappable.cpp

	)

SET( SRC ${SRC} PARENT_SCOPE )
