#include "Difficulty.h"

#include <algorithm>

#include "Rules.h"

namespace task {
namespace mainmenu {

Difficulty::Difficulty( MainMenu *mainmenu )
	: SlidingMenu( mainmenu, "PICK A DIFFICULTY LEVEL", GenerateChoices( mainmenu ), GetDefaultChoice( mainmenu ) )
{
	//
}

const MenuBlock::choices_t Difficulty::GenerateChoices( MainMenu *mainmenu ) {
	MenuBlock::choices_t choices = {};
	auto& rules = mainmenu->m_settings.global.game_rules;
	for ( auto& it : rules.m_difficulty_levels ) {
		auto name = it.second.m_name;
		std::transform( name.begin(), name.end(), name.begin(), ::toupper );
		choices.push_back({ name, {
			CH( this, it, mainmenu ) {
				mainmenu->m_settings.global.global_difficulty = it.second;
				NEWV( menu, Rules, mainmenu );
				NextMenu( menu );
			}
		}});
	}
	return choices;
}

const size_t Difficulty::GetDefaultChoice( MainMenu *mainmenu ) const {
	auto& rules = mainmenu->m_settings.global.game_rules;
	const auto& default_difficulty_level = rules.GetDefaultDifficultyLevel();
	size_t choice_idx = 0;
	for ( auto& it : rules.m_difficulty_levels ) {
		if ( it.second.m_difficulty == default_difficulty_level.m_difficulty ) {
			return choice_idx;
		}
		choice_idx++;
	}
	return 0;
}

}
}
