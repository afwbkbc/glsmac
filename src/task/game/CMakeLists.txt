SUBDIR( ui )
SUBDIR( actor )

SET( SRC ${SRC}

	${PWD}/Game.cpp
	${PWD}/InstancedSpriteManager.cpp
	${PWD}/InstancedSprite.cpp
	${PWD}/Sprite.cpp
	${PWD}/BadgeDefs.cpp
	${PWD}/Slot.cpp
	${PWD}/SlotBadges.cpp
	${PWD}/UnitDef.cpp
	${PWD}/Unit.cpp
	${PWD}/Tile.cpp

	PARENT_SCOPE )
