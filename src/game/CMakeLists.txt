SUBDIR( world )
SUBDIR( map_editor )
SUBDIR( rules )
SUBDIR( connection )

SET( SRC ${SRC}

	${PWD}/Game.cpp
	${PWD}/State.cpp
	${PWD}/Settings.cpp
	${PWD}/Account.cpp
	${PWD}/Player.cpp
	${PWD}/Turn.cpp
	${PWD}/Slot.cpp
	${PWD}/Slots.cpp
	${PWD}/Event.cpp
	${PWD}/Bindings.cpp

	PARENT_SCOPE )
