SUBDIR( map )
SUBDIR( map_editor )
SUBDIR( rules )
SUBDIR( connection )

SET( SRC ${SRC}

	${PWD}/Game.cpp
	${PWD}/State.cpp
	${PWD}/Settings.cpp
	${PWD}/Account.cpp
	${PWD}/Player.cpp
	${PWD}/Slot.cpp
	${PWD}/Slots.cpp
	${PWD}/Event.cpp

	PARENT_SCOPE )
