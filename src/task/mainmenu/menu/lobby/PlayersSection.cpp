#include "PlayersSection.h"

#include "engine/Engine.h"

#include "game/Slot.h"

namespace task {
namespace mainmenu {
namespace lobby {

PlayersSection::PlayersSection( Lobby* lobby )
	: LobbySection( lobby ) {
	SetTitleText( "PLAYERS" );
	SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_TOP );
}

void PlayersSection::Create() {
	LobbySection::Create();

}

void PlayersSection::Align() {
	SetWidth( 496 );
	SetHeight( 212 );

	LobbySection::Align();
}

void PlayersSection::Destroy() {

	for ( auto& row : m_slots ) {
		RemoveChild( row );
	}

	LobbySection::Destroy();
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

void PlayersSection::UpdateSlots( std::vector< ::game::Slot >& slots ) {
	const auto& game_rules = GetLobby()->GetSettings().global.game_rules;

	m_choices.factions.clear();
	for ( auto& faction : game_rules.m_factions ) {
		m_choices.factions.push_back(
			{
				faction.first,
				faction.second.m_name
			}
		);
	}

	m_choices.difficulty_levels.clear();
	for ( auto& difficulty : game_rules.m_difficulty_levels ) {
		m_choices.difficulty_levels.push_back(
			{
				difficulty.first,
				difficulty.second.m_name
			}
		);
	}

	// TODO: more granular updates
	for ( size_t i = 0 ; i < slots.size() ; i++ ) {
		UpdateSlot( i, &( slots.at( i ) ) );
	}
}

const ChoiceList::choices_t& PlayersSection::GetFactionChoices() {
	return m_choices.factions;
}

const ChoiceList::choices_t& PlayersSection::GetDifficultyLevelChoices() {
	return m_choices.difficulty_levels;
}

}
}
}
