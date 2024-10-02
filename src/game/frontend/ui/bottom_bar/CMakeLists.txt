SUBDIR( left_menu )
SUBDIR( right_menu )
SUBDIR( objects_list )

SET( SRC ${SRC}

	${PWD}/BottomBarBase.cpp
	${PWD}/BottomBar.cpp
	${PWD}/ObjectPreview.cpp
	${PWD}/TilePreview.cpp
	${PWD}/MiddleArea.cpp
	${PWD}/MiddleAreaPage.cpp
	${PWD}/Messages.cpp
	${PWD}/Toolbar.cpp
	${PWD}/InfoPanels.cpp
	${PWD}/InfoPanel.cpp
	${PWD}/MiniMap.cpp
	${PWD}/StatusButton.cpp

	PARENT_SCOPE )
