SUBDIR( left_menu )
SUBDIR( right_menu )
SUBDIR( units_list )

SET( SRC ${SRC}

	${PWD}/BottomBar.cpp
	${PWD}/UnitPreview.cpp
	${PWD}/TilePreview.cpp
	${PWD}/MiddleArea.cpp
	${PWD}/MiddleAreaPage.cpp
	${PWD}/Messages.cpp
	${PWD}/Toolbar.cpp
	${PWD}/InfoPanels.cpp
	${PWD}/InfoPanel.cpp
	${PWD}/MiniMap.cpp
	${PWD}/TurnCompleteButton.cpp

	PARENT_SCOPE )
