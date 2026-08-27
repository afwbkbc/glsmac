IF ( CMAKE_CONFIGURATION_TYPES )
	IF ( BUILD_TESTING )
		SET( SRC ${SRC}
			${PWD}/Mocks.cpp
			${PWD}/Test.cpp
			PARENT_SCOPE )
	ELSE ()
		SET( SRC ${SRC}
			$<$<OR:$<CONFIG:Debug>,$<CONFIG:FastDebug>>:${PWD}/Mocks.cpp>
			$<$<OR:$<CONFIG:Debug>,$<CONFIG:FastDebug>>:${PWD}/Test.cpp>
			PARENT_SCOPE )
	ENDIF ()
ELSE ()
	SET( SRC ${SRC}
		${PWD}/Mocks.cpp
		${PWD}/Test.cpp
		PARENT_SCOPE )
ENDIF ()
