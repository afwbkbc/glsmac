#include "Default.h"

namespace game {
namespace rules {

void Default::InitRules() {
	
	m_factions = {
		
		{ 1, {
			"Faction One",
			types::Color::FromRGB( 255, 0, 0 )
		} },
		
		{ 2, {
			"Faction Two",
			types::Color::FromRGB( 0, 255, 0 )
		} },
		
		{ 3, {
			"Faction Three",
			types::Color::FromRGB( 0, 0, 255 )
		} },
		
	};
	
}

}
}
