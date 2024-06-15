SET( SRC ${SRC}

	${PWD}/Event.cpp
	${PWD}/DefineAnimation.cpp
	${PWD}/DefineMorales.cpp
	${PWD}/DefineUnit.cpp
	${PWD}/SpawnUnit.cpp
	${PWD}/DespawnUnit.cpp
	${PWD}/MoveUnit.cpp
	${PWD}/AttackUnit.cpp
	${PWD}/SkipUnitTurn.cpp
	${PWD}/SpawnBase.cpp
	${PWD}/CompleteTurn.cpp
	${PWD}/UncompleteTurn.cpp
	${PWD}/FinalizeTurn.cpp
	${PWD}/TurnFinalized.cpp
	${PWD}/AdvanceTurn.cpp
	${PWD}/TileLocksEvent.cpp
	${PWD}/RequestTileLocks.cpp
	${PWD}/LockTiles.cpp
	${PWD}/RequestTileUnlocks.cpp
	${PWD}/UnlockTiles.cpp

	PARENT_SCOPE )
