#pragma once

#include "../SlidingMenu.h"

namespace task {
namespace mainmenu {
	
CLASS( Erosive, SlidingMenu )
	Erosive( MainMenu *mainmenu );

	void Close();

private:
	void SetMoonsVariant( const char variant );
	
};

}
}
