SUBDIR( random )
SUBDIR( crc32 )

SET( SRC ${SRC}

	${PWD}/Timer.cpp
	${PWD}/System.cpp
	${PWD}/Math.cpp
	${PWD}/Perlin.cpp
	${PWD}/FS.cpp
	${PWD}/UUID.cpp
	${PWD}/ConfigManager.cpp
	${PWD}/String.cpp

	PARENT_SCOPE )
