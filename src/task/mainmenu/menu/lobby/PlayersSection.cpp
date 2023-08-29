#include "PlayersSection.h"

#include "engine/Engine.h"

#include "game/Slot.h"

namespace task {
namespace mainmenu {
namespace lobby {

PlayersSection::PlayersSection( Lobby* lobby )
	: LobbySection( lobby )
{
	ApplyRules();
	SetTitleText( "PLAYERS" );
	SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
}

void PlayersSection::Create() {
	Section::Create();
	
}

void PlayersSection::Align() {
	SetWidth( 496 );
	SetHeight( 212 );

	Section::Align();
}

void PlayersSection::Destroy() {
	
	for ( auto& row : m_slots ) {
		RemoveChild( row );
	}

	Section::Destroy();
}

void PlayersSection::UpdateSlot( const size_t slot_num, ::game::Slot* slot ) {
	ASSERT( slot, "updateslot with null slot" );
	ASSERT( slot_num <= m_slots.size(), "slot num overflow ( " + std::to_string( slot_num ) + " > " + std::to_string( m_slots.size() ) + " )" );
	NEWV( row, PlayersSectionRow, this, slot_num, slot );
		row->SetHeight( 20 );
		row->SetTop( 22 * slot_num + 1 );
	if ( slot_num == m_slots.size() ) {
		m_slots.push_back( row );
	}
	else {
		RemoveChild( m_slots[ slot_num ] );
		m_slots[ slot_num ] = row;
	}
	AddChild( row );
}

const ChoiceList::choices_t& PlayersSection::GetFactionChoices() {
	return m_choices.factions;
}

const ChoiceList::choices_t& PlayersSection::GetDifficultyLevelChoices() {
	return m_choices.difficulty_levels;
}

void PlayersSection::ApplyRules() {
	const auto& game_rules = GetLobby()->GetSettings().global.game_rules;
	
	m_choices.factions.clear();
	for ( auto& faction : game_rules.m_factions ) {
		m_choices.factions.push_back({ faction.first, faction.second.m_name });
	}
	
	m_choices.difficulty_levels.clear();
	for ( auto& difficulty : game_rules.m_difficulty_levels ) {
		m_choices.difficulty_levels.push_back({ difficulty.first, difficulty.second.m_name });
	}
}

}
}
}
