SUBDIR( generator )
SUBDIR( module )

SET( SRC ${SRC}

	${PWD}/Consts.cpp
	${PWD}/World.cpp
	${PWD}/Tiles.cpp
	${PWD}/Tile.cpp
	${PWD}/MapState.cpp
	${PWD}/TileState.cpp

	PARENT_SCOPE )
