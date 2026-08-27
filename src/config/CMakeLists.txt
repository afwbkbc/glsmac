SET( SRC ${SRC}

	${PWD}/Config.cpp
	${PWD}/WrappedConfig.cpp

)

IF ( WIN32 )
	SET( SRC ${SRC}

		${PWD}/WindowsInstallPaths.cpp

	)
ENDIF ()

SET( SRC ${SRC} PARENT_SCOPE )
