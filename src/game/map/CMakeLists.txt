SUBDIR( generator )
SUBDIR( module )

SET( SRC ${SRC}

	${PWD}/Consts.cpp
	${PWD}/Map.cpp
	${PWD}/Tiles.cpp
	${PWD}/Tile.cpp
	${PWD}/MapState.cpp
	${PWD}/TileState.cpp

	PARENT_SCOPE )
