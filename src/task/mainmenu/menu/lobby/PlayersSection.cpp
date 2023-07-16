#include "PlayersSection.h"

#include "engine/Engine.h"

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
	
	//ClearSlots();

	Section::Destroy();
}

void PlayersSection::UpdateSlots( const std::vector< ::game::Slot >& slots ) {
	size_t slots_i = 0;
	size_t rows_i = 0;
	while ( slots_i++ < slots.size() && rows_i++ < m_player_rows.size() ) {
		Log( "SLOTS_I = " + std::to_string( slots_i ) + " ROWS_I = " + std::to_string( rows_i ) );
	}
	
}


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
