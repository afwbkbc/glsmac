#include "PlayersSection.h"

#include "engine/Engine.h"

#include "game/Slot.h"

namespace task {
namespace mainmenu {
namespace lobby {

PlayersSection::PlayersSection( Lobby* lobby )
	: Section( "PopupSection" )
	, m_lobby( lobby )
{
	ApplyRules();
}

void PlayersSection::Create() {
	Section::Create();
	
}

void PlayersSection::Destroy() {
	
	for ( auto& row : m_slots ) {
		RemoveChild( row );
	}

	Section::Destroy();
}

void PlayersSection::UpdateSlot( const size_t slot_num, const ::game::Slot* slot ) {
	ASSERT( slot, "updateslot with null slot" );
	ASSERT( slot_num <= m_slots.size(), "slot num overflow ( " + std::to_string( slot_num ) + " > " + std::to_string( m_slots.size() ) + " )" );
	NEWV( row, PlayersSectionRow, this, slot );
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

/*void PlayersSection::UpdateSlots( const std::vector< ::game::Slot >& slots ) {
	size_t slots_i = 0;
	size_t rows_i = 0;
	while ( slots_i++ < slots.size() && rows_i++ < m_player_rows.size() ) {
		Log( "SLOTS_I = " + std::to_string( slots_i ) + " ROWS_I = " + std::to_string( rows_i ) );
	}
	
}*/


/*
void PlayersSection::ClearPlayers() {
	for ( auto& row : m_player_rows ) {
		RemoveChild( row.second );
	}
	m_player_rows.clear();
}

void PlayersSection::SetPlayers( const players_t& players ) {
	ClearPlayers();
	size_t top = 0;
	for ( auto& player : players ) {
		NEWV( player_row, PlayersSectionRow, this, player.first, player.second );
			player_row->SetHeight( 20 );
			player_row->SetTop( top + 1 );
		AddChild( player_row );
		m_player_rows[ player.first ] = player_row;
		
		
		top += 22;
	}
}

void PlayersSection::UpdatePlayer( const size_t cid, const ::game::Player& player ) {
	m_player_rows.at( cid )->Update( player );
}
*/
Lobby* PlayersSection::GetLobby() {
	return m_lobby;
}

const ChoiceList::choices_t& PlayersSection::GetFactionChoices() {
	return m_choices.factions;
}

const ChoiceList::choices_t& PlayersSection::GetDifficultyLevelChoices() {
	return m_choices.difficulty_levels;
}

void PlayersSection::ApplyRules() {
	const auto& game_rules = m_lobby->GetSettings().global.game_rules;
	
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
