SUBDIR( map )
SUBDIR( map_editor )
SUBDIR( rules )
SUBDIR( connection )
SUBDIR( unit )
SUBDIR( bindings )
SUBDIR( event )

SET( SRC ${SRC}

	${PWD}/Game.cpp
	${PWD}/State.cpp
	${PWD}/Settings.cpp
	${PWD}/Account.cpp
	${PWD}/Player.cpp
	${PWD}/Slot.cpp
	${PWD}/Slots.cpp
	${PWD}/Turn.cpp
	${PWD}/Event.cpp

	PARENT_SCOPE )
