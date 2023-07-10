#include "PlayersSectionRow.h"

#include "engine/Engine.h"

namespace task {
namespace mainmenu {
namespace lobby {

PlayersSectionRow::PlayersSectionRow( PlayersSection* parent, const size_t cid, const ::game::Player& player, const std::string& class_name )
	: UIContainer( class_name )
	, m_parent( parent )
	, m_cid( cid )
	, m_player( player )
{
	//
}

void PlayersSectionRow::Create() {
	UIContainer::Create();
	
	NEW( m_elements.actions, Dropdown, "PopupDropdown" );
		m_elements.actions->SetMode( Dropdown::DM_MENU );
		m_elements.actions->SetChoicesV({
			"Customize faction",
		});
		m_elements.actions->SetLeft( 36 );
		m_elements.actions->SetWidth( 178 );
		m_elements.actions->On( UIEvent::EV_CHANGE, EH( this ) {
			const auto& value = *(data->value.change.text);
			if ( value == "Customize faction" ) {
				g_engine->GetUI()->GetError()->Show( "This feature is not available yet." );
			}
			else {
				ASSERT( false, "invalid selection" );
			}
			return true;
		});
	AddChild( m_elements.actions );
	
	NEW( m_elements.faction, Dropdown, "PopupDropdown" );
		m_elements.faction->SetChoices( m_parent->GetFactionChoices() );
		m_elements.faction->SetValue( m_parent->GetFactionChoices().front().second ); // first faction (random) as default
		m_elements.faction->SetLeft( 218 );
		m_elements.faction->SetWidth( 140 );
		m_elements.faction->On( UIEvent::EV_CHANGE, EH( this ) {
			m_player.SetFaction( m_parent->GetLobby()->GetSettings().global.game_rules.m_factions[ data->value.change.id ] );
			m_parent->GetLobby()->UpdatePlayer( m_cid, m_player );
			return true;
		});
	AddChild( m_elements.faction );
	
	NEW( m_elements.difficulty_level, Dropdown, "PopupDropdown" );
		m_elements.difficulty_level->SetChoices( m_parent->GetDifficultyLevelChoices() );
		m_elements.difficulty_level->SetValue( m_parent->GetDifficultyLevelChoices().back().second ); // last difficulty (transcend) as default
		m_elements.difficulty_level->SetLeft( 360 );
		m_elements.difficulty_level->SetWidth( 118 );
		m_elements.difficulty_level->On( UIEvent::EV_CHANGE, EH( this ) {
			Log( "SELECTED DIFFICULTY LEVEL: " + *(data->value.change.text) + " (" + std::to_string( data->value.change.id ) + ")" );
			return true;
		});
	AddChild( m_elements.difficulty_level );
	
	Update( m_player );
}

void PlayersSectionRow::Destroy() {
	
	RemoveChild( m_elements.actions );
	RemoveChild( m_elements.faction );
	RemoveChild( m_elements.difficulty_level );
	
	UIContainer::Destroy();
}

void PlayersSectionRow::Update( const ::game::Player& player ) {
	m_player = player;
	if ( m_elements.actions ) {
		m_elements.actions->SetValue( m_player.GetName() );
		m_elements.actions->SetTextColor( m_player.GetFaction().m_color );
	}
	if ( m_elements.faction ) {
		m_elements.faction->SetValue( m_player.GetFaction().m_name );
		m_elements.faction->SetTextColor( m_player.GetFaction().m_color );
	}
	if ( m_elements.difficulty_level ) {
		m_elements.difficulty_level->SetTextColor( m_player.GetFaction().m_color );
	}
}

}
}
}
