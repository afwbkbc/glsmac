#pragma once

#include "task/mainmenu/SlidingMenu.h"

namespace task {
namespace mainmenu {

CLASS( Difficulty, SlidingMenu )
	Difficulty( MainMenu* mainmenu );

private:
	const choices_t GenerateChoices( MainMenu* mainmenu );
	const size_t GetDefaultChoice( MainMenu* mainmenu ) const;
};

}
}
