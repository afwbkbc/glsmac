SUBDIR( ui )
SUBDIR( actor )
SUBDIR( faction )
SUBDIR( unit )

SET( SRC ${SRC}

	${PWD}/Game.cpp
	${PWD}/InstancedSpriteManager.cpp
	${PWD}/InstancedSprite.cpp
	${PWD}/Sprite.cpp
	${PWD}/Slot.cpp
	${PWD}/AnimationDef.cpp
	${PWD}/Animation.cpp
	${PWD}/Tile.cpp

	PARENT_SCOPE )
