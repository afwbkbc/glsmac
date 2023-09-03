#include "Default.h"

using namespace types;

namespace game {
namespace rules {

void Default::InitRules() {
	
	m_factions = {
		
		// SMAC
		
		{ FT_RANDOM, {
			"Random",
			Color::FromRGB( 255, 255, 255 )
		} },
		
		{ FT_GAIANS, {
			"Gaians",
			Color::FromRGB( 16, 228, 0 )
		} },
		
		{ FT_HIVE, {
			"Hive",
			Color::FromRGB( 0, 97, 235 )
		} },
		
		{ FT_UNIVERSITY, {
			"University",
			Color::FromRGB( 216, 224, 235 )
		} },
		
		{ FT_MORGANITES, {
			"Morganites",
			Color::FromRGB( 255, 255, 0 )
		} },
		
		{ FT_SPARTANS, {
			"Spartans",
			Color::FromRGB( 137, 166, 166 )
		} },
		
		{ FT_BELIEVERS, {
			"Believers",
			Color::FromRGB( 224, 156, 28 )
		} },
		
		{ FT_PEACEKEEPERS, {
			"Peacekeepers",
			Color::FromRGB( 164, 176, 232 )
		} },
		
		// SMACX
		
		{ FT_CONSCIOUSNESS, {
			"Consciousness",
			Color::FromRGB( 44, 128, 104 )
		} },
		
		{ FT_PIRATES, {
			"Pirates",
			Color::FromRGB( 0, 255, 255 )
		} },
		
		{ FT_DRONES, {
			"Drones",
			Color::FromRGB( 173, 196, 192 )
		} },
		
		{ FT_ANGELS, {
			"Angels",
			Color::FromRGB( 103, 91, 181 )
		} },
		
		{ FT_PLANETCULT, {
			"Planet Cult",
			Color::FromRGB( 232, 84, 84 )
		} },
		
		{ FT_CARETAKERS, {
			"Caretakers",
			Color::FromRGB( 116, 156, 56 )
		} },
		
		{ FT_USURPERS, {
			"Usurpers",
			Color::FromRGB( 212, 208, 116 )
		} },
	};

	m_difficulty_levels = {
		{ DT_CITIZEN, { "Citizen", -3 } },
		{ DT_SPECIALIST, { "Specialist", -2 } },
		{ DT_TALENT, { "Talent", -1 } },
		{ DT_LIBRARIAN, { "Librarian", 0 } },
		{ DT_THINKER, { "Thinker", 1 } },
		{ DT_TRANSCEND, { "Transcend", 2 } },
	};

}

const Faction& Default::GetDefaultFaction() const {
	return m_factions.at( FT_RANDOM );
}

const DifficultyLevel& Default::GetDefaultDifficultyLevel() const {
	return m_difficulty_levels.at( DT_TRANSCEND );
}

}
}
