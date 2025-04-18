SUBDIR( map )
SUBDIR( map_editor )
SUBDIR( resource )
SUBDIR( rules )
SUBDIR( settings )
SUBDIR( slot )
SUBDIR( connection )
SUBDIR( faction )
SUBDIR( animation )
SUBDIR( unit )
SUBDIR( base )
SUBDIR( event )
SUBDIR( turn )

SET( SRC ${SRC}

	${PWD}/Random.cpp
	${PWD}/Bindings.cpp
	${PWD}/State.cpp
	${PWD}/Game.cpp
	${PWD}/Event.cpp
	${PWD}/Account.cpp
	${PWD}/Player.cpp
	${PWD}/MapObject.cpp

	PARENT_SCOPE )
