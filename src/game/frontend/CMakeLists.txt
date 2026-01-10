SUBDIR( ui_legacy )
SUBDIR( actor )
SUBDIR( faction )
SUBDIR( tile )
SUBDIR( sprite )
SUBDIR( text )
SUBDIR( unit )
SUBDIR( base )
SUBDIR( widget )

SET( SRC ${SRC}

	${PWD}/Game.cpp
	${PWD}/Slot.cpp
	${PWD}/TileObject.cpp
	${PWD}/AnimationDef.cpp
	${PWD}/Animation.cpp

	PARENT_SCOPE )
