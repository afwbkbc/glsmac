#pragma once

#include "task/mainmenu/SlidingMenu.h"

namespace task {
namespace mainmenu {

CLASS( Erosive, SlidingMenu )
	Erosive( MainMenu* mainmenu );

	void Close() override;

private:
	void SetMoonsVariant( const char variant );

};

}
}
