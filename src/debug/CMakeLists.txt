IF ( CMAKE_CONFIGURATION_TYPES )
	SET( SRC ${SRC}
		$<$<CONFIG:Debug>:${PWD}/MemoryWatcher.cpp>
		PARENT_SCOPE )
ELSE ()
	SET( SRC ${SRC}
		${PWD}/MemoryWatcher.cpp
		PARENT_SCOPE )
ENDIF ()
