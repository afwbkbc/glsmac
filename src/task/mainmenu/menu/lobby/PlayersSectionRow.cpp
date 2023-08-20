#include "PlayersSectionRow.h"

#include "engine/Engine.h"

namespace task {
namespace mainmenu {
namespace lobby {

PlayersSectionRow::PlayersSectionRow( PlayersSection* parent, const ::game::Slot* slot, const std::string& class_name )
	: UIContainer( class_name )
	, m_parent( parent )
	, m_slot( slot )
{
	//
}

void PlayersSectionRow::Create() {
	UIContainer::Create();
	
	NEW( m_elements.actions, Dropdown, "PopupDropdown" );
		m_elements.actions->SetMode( Dropdown::DM_MENU );
		m_elements.actions->SetLeft( 36 );
		m_elements.actions->SetWidth( 178 );
	AddChild( m_elements.actions );
	
	switch ( m_slot->GetState() ) {
		case ::game::Slot::SS_OPEN: {
			m_elements.actions->SetValue( "Open" );
			break;
		}
		case ::game::Slot::SS_CLOSED: {
			m_elements.actions->SetValue( "Closed" );
			break;
		}
		case ::game::Slot::SS_PLAYER: {
			auto* player = m_slot->GetPlayer();
			ASSERT( player, "slot player is null" );
			
			const auto& rules = m_parent->GetLobby()->GetSettings().global.game_rules;
			
			NEW( m_elements.faction, Dropdown, "PopupDropdown" );
				m_elements.faction->SetChoices( m_parent->GetFactionChoices() );
				m_elements.faction->SetValue( player->GetFaction().m_name );
				m_elements.faction->SetLeft( 218 );
				m_elements.faction->SetWidth( 140 );
				m_elements.faction->On( UIEvent::EV_CHANGE, EH( this, player, rules ) {
					player->SetFaction( rules.m_factions.at( data->value.change.id ) );
					return true;
				});
			AddChild( m_elements.faction );

			NEW( m_elements.difficulty_level, Dropdown, "PopupDropdown" );
				m_elements.difficulty_level->SetChoices( m_parent->GetDifficultyLevelChoices() );
				m_elements.difficulty_level->SetValue( player->GetDifficultyLevel().m_name );
				m_elements.difficulty_level->SetLeft( 360 );
				m_elements.difficulty_level->SetWidth( 118 );
				m_elements.difficulty_level->On( UIEvent::EV_CHANGE, EH( this, player, rules ) {
					player->SetDifficultyLevel( rules.m_difficulty_levels.at( data->value.change.id ) );
					return true;
				});
			AddChild( m_elements.difficulty_level );

			m_elements.actions->SetValue( player->GetPlayerName() );
			m_elements.actions->SetTextColor( player->GetFaction().m_color );
			m_elements.faction->SetValue( player->GetFaction().m_name );
			m_elements.faction->SetTextColor( player->GetFaction().m_color );
			m_elements.difficulty_level->SetTextColor( player->GetFaction().m_color );
			
			const auto* connection = m_parent->GetLobby()->GetConnection();
			ASSERT( connection, "connection is null" );
			
			auto* me = connection->GetPlayer();
			const bool is_it_me = m_slot->GetPlayer() == me;
			const bool am_i_host = me->GetRole() == ::game::Player::PR_HOST;
			
			std::vector< std::string > actions = {};
			if ( is_it_me ) {
				actions.push_back( "Customize faction" );
			}
			else {
				if ( am_i_host ) {
					actions.push_back( "Kick" );
					actions.push_back( "Ban" );
				}
			}
			
			m_elements.actions->SetChoicesV(actions);
			m_elements.actions->On( UIEvent::EV_CHANGE, EH( this, is_it_me, am_i_host ) {
				const auto& value = *(data->value.change.text);
				if ( is_it_me ) {
					if ( value == "Customize faction" ) {
						g_engine->GetUI()->GetError()->Show( "This feature is not available yet." );
						return true;
					}
				}
				else {
					if ( am_i_host ) {
						if ( value == "Kick" ) {
							g_engine->GetUI()->GetError()->Show( "This feature is not available yet." );
							return true;
						}
						if ( value == "Ban" ) {
							g_engine->GetUI()->GetError()->Show( "This feature is not available yet." );
							return true;
						}
					}
				}
				ASSERT( false, "invalid selection" );
				return false;
			});

		}
	}
	
}

void PlayersSectionRow::Destroy() {
	
	RemoveChild( m_elements.actions );
	
	if ( m_elements.faction ) {
		RemoveChild( m_elements.faction );
	}
	
	if ( m_elements.difficulty_level ) {
		RemoveChild( m_elements.difficulty_level );
	}
	
	UIContainer::Destroy();
}
}

}
}
