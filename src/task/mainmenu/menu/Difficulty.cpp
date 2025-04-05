#include "Difficulty.h"

#include <algorithm>

#include "Rules.h"

#include "task/mainmenu/MainMenu.h"
#include "game/backend/State.h"
#include "game/backend/settings/Settings.h"

namespace task {
namespace mainmenu {

Difficulty::Difficulty( MainMenu* mainmenu )
	: SlidingMenu( mainmenu, "PICK A DIFFICULTY LEVEL", GenerateChoices( mainmenu ), GetDefaultChoice( mainmenu ) ) {
	//
}

const choices_t Difficulty::GenerateChoices( MainMenu* mainmenu ) {
	choices_t choices = {};
	auto& rules = mainmenu->m_state->m_settings.global.rules;
	for ( auto& it : rules.m_difficulty_levels.GetKV() ) {
		auto name = it.first;
		std::transform( name.begin(), name.end(), name.begin(), ::toupper );
		choices.push_back(
			{
				name,
				{
					CH( this, it, mainmenu ) {
						mainmenu->m_state->m_settings.global.difficulty_level = it.second;
						NEWV( menu, Rules, mainmenu );
						NextMenu( menu );
					}
				}
			}
		);
	}
	return choices;
}

const size_t Difficulty::GetDefaultChoice( MainMenu* mainmenu ) const {
	auto& rules = mainmenu->m_state->m_settings.global.rules;
	const auto& default_difficulty_level = rules.GetDefaultDifficultyLevel();
	size_t choice_idx = 0;
	for ( auto& it : rules.m_difficulty_levels.GetKV() ) {
		if ( it.first == default_difficulty_level ) {
			return choice_idx;
		}
		choice_idx++;
	}
	return 0;
}

}
}
