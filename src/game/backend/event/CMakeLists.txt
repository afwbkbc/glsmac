SET( SRC ${SRC}

	${PWD}/LegacyEvent.cpp
	${PWD}/DefineResource.cpp
	${PWD}/DefineAnimation.cpp
	${PWD}/DefineMorales.cpp
	${PWD}/DefineUnit.cpp
	${PWD}/MoveUnit.cpp
	${PWD}/AttackUnit.cpp
	${PWD}/SkipUnitTurn.cpp
	${PWD}/DefinePop.cpp
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
	${PWD}/Event.cpp
	${PWD}/EventHandler.cpp

	PARENT_SCOPE )
