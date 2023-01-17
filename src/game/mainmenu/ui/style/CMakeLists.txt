SET(SRC ${SRC}

	${PWD}/MainMenuBackground.cpp

	${PWD}/SlidingMenuBlock.cpp
		${PWD}/SlidingMenuItem.cpp
			${PWD}/SlidingMenuItemUpperTop.cpp
			${PWD}/SlidingMenuItemLowerTop.cpp
			${PWD}/SlidingMenuItemLeft.cpp
			${PWD}/SlidingMenuItemRight.cpp
			${PWD}/SlidingMenuItemBottom.cpp
			${PWD}/SlidingMenuItemBody.cpp
			${PWD}/SlidingMenuItemText.cpp
				${PWD}/SlidingMenuButtonText.cpp
				${PWD}/SlidingMenuTitleText.cpp

	${PWD}/PopupMenuFrame.cpp
	${PWD}/PopupMenuButton.cpp

PARENT_SCOPE)
