SUBDIR( map )
SUBDIR( map_editor )
SUBDIR( rules )
SUBDIR( settings )
SUBDIR( slot )
SUBDIR( connection )
SUBDIR( faction )
SUBDIR( animation )
SUBDIR( unit )
SUBDIR( base )
SUBDIR( bindings )
SUBDIR( event )
SUBDIR( turn )

SET( SRC ${SRC}

	${PWD}/System.cpp
	${PWD}/Game.cpp
	${PWD}/State.cpp
	${PWD}/Account.cpp
	${PWD}/Player.cpp
	${PWD}/MapObject.cpp
	${PWD}/Resource.cpp

	PARENT_SCOPE )
