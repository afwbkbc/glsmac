#pragma once

#include "../SlidingMenu.h"

namespace task {
namespace mainmenu {

CLASS( Difficulty, SlidingMenu )
	Difficulty( MainMenu* mainmenu );

private:
	const MenuBlock::choices_t GenerateChoices( MainMenu* mainmenu );
	const size_t GetDefaultChoice( MainMenu* mainmenu ) const;
};

}
}
