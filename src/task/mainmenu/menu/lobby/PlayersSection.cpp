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
	
	ClearPlayers();
	
	Section::Destroy();
}

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
		
		NEWV( player_row, PlayersSectionRow, this, player.second );
			player_row->SetHeight( 20 );
			player_row->SetTop( top + 1 );
		AddChild( player_row );
		m_player_rows[ player.first ] = player_row;
		
		top += 22;
	}
}

Lobby* PlayersSection::GetLobby() {
	return m_lobby;
}

const std::vector< std::string >& PlayersSection::GetFactionChoices() {
	return m_choices.factions;
}

const std::vector< std::string >& PlayersSection::GetDifficultyLevelChoices() {
	return m_choices.difficulty_levels;
}

void PlayersSection::ApplyRules() {
	const auto& game_rules = m_lobby->GetSettings().global.game_rules;
	
	m_choices.factions = { "Random" };
	for ( auto& faction : game_rules.m_factions ) {
		m_choices.factions.push_back( faction.second.m_name );
	}
	
	m_choices.difficulty_levels = {};
	for ( auto& difficulty : game_rules.m_difficulty_levels ) {
		m_choices.difficulty_levels.push_back( difficulty.second.m_name );
	}
}

}
}
}
