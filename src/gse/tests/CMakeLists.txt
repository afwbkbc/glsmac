SUBDIR( mocks )

IF ( CMAKE_CONFIGURATION_TYPES )
	SET( SRC ${SRC}
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:FastDebug>>:${PWD}/Tests.cpp>
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:FastDebug>>:${PWD}/GSE.cpp>
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:FastDebug>>:${PWD}/Parser.cpp>
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:FastDebug>>:${PWD}/Runner.cpp>
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:FastDebug>>:${PWD}/Scripts.cpp>
		PARENT_SCOPE )
ELSE ()
	SET( SRC ${SRC}
		${PWD}/Tests.cpp
		${PWD}/GSE.cpp
		${PWD}/Parser.cpp
		${PWD}/Runner.cpp
		${PWD}/Scripts.cpp
		PARENT_SCOPE )
ENDIF ()
