#include "Default.h"

using namespace types;

namespace game {
namespace rules {

void Default::InitRules() {
	
	m_factions = {
		
		// SMAC
		
		{ 1, {
			"Gaians",
			Color::FromRGB( 16, 228, 0 )
		} },
		
		{ 2, {
			"Hive",
			Color::FromRGB( 0, 97, 235 )
		} },
		
		{ 3, {
			"University",
			Color::FromRGB( 216, 224, 235 )
		} },
		
		{ 4, {
			"Morganites",
			Color::FromRGB( 255, 255, 0 )
		} },
		
		{ 5, {
			"Spartans",
			Color::FromRGB( 137, 166, 166 )
		} },
		
		{ 6, {
			"Believers",
			Color::FromRGB( 224, 156, 28 )
		} },
		
		{ 7, {
			"Peacekeepers",
			Color::FromRGB( 164, 176, 232 )
		} },
		
		// SMACX
		
		{ 8, {
			"Consciousness",
			Color::FromRGB( 44, 128, 104 )
		} },
		
		{ 9, {
			"Pirates",
			Color::FromRGB( 0, 255, 255 )
		} },
		
		{ 10, {
			"Drones",
			Color::FromRGB( 173, 196, 192 )
		} },
		
		{ 11, {
			"Angels",
			Color::FromRGB( 103, 91, 181 )
		} },
		
		{ 12, {
			"Planet Cult",
			Color::FromRGB( 232, 84, 84 )
		} },
		
		{ 13, {
			"Caretakers",
			Color::FromRGB( 116, 156, 56 )
		} },
		
		{ 14, {
			"Usurpers",
			Color::FromRGB( 212, 208, 116 )
		} },
	};
	
	m_difficulty_levels = {
		
		{ 1, { "Citizen", -3 } },
		{ 2, { "Specialist", -2 } },
		{ 3, { "Talent", -1 } },
		{ 4, { "Librarian", 0 } },
		{ 5, { "Thinker", 1 } },
		{ 6, { "Transcend", 2 } },
		
	};
	
}

}
}
