SUBDIR( ui )
SUBDIR( actor )
SUBDIR( faction )
SUBDIR( tile )
SUBDIR( unit )
SUBDIR( base )

SET( SRC ${SRC}

	${PWD}/Game.cpp
	${PWD}/InstancedSpriteManager.cpp
	${PWD}/InstancedSprite.cpp
	${PWD}/Sprite.cpp
	${PWD}/Slot.cpp
	${PWD}/AnimationDef.cpp
	${PWD}/Animation.cpp

	PARENT_SCOPE )
