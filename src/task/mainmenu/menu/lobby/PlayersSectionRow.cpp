#include "PlayersSectionRow.h"

#include "engine/Engine.h"

namespace task {
namespace mainmenu {
namespace lobby {

PlayersSectionRow::PlayersSectionRow( PlayersSection* parent, const size_t slot_num, ::game::Slot* slot, const std::string& class_name )
	: UIContainer( class_name )
	, m_parent( parent )
	, m_slot_num( slot_num )
	, m_slot( slot ) {
	//
}

void PlayersSectionRow::Create() {
	UIContainer::Create();

	auto* me = m_parent->GetLobby()->GetPlayer();
	const bool am_i_host = me->GetRole() == ::game::Player::PR_HOST;

	NEW( m_elements.actions, Dropdown, "PopupDropdown" );
	m_elements.actions->SetLeft( 36 );
	m_elements.actions->SetWidth( 178 );

	if ( m_slot->GetState() == ::game::Slot::SS_PLAYER ) {
		m_elements.actions->SetMode( Dropdown::DM_MENU );

		auto* player = m_slot->GetPlayer();
		ASSERT( player, "slot player is null" );

		const bool is_it_me = player == me;

		if ( is_it_me ) {
			ui::object::UIContainer::AddStyleModifier( Style::M_HIGHLIGHT );
		}

		const auto& rules = m_parent->GetLobby()->GetSettings().global.game_rules;

		const auto& faction_color = player->GetFaction().m_color;

		if ( m_slot->IsReady() ) {
			NEW( m_elements.ready, Surface );
			m_elements.ready->SetWidth( 24 );
			m_elements.ready->SetHeight( 16 );
			m_elements.ready->SetLeft( 8 );
			m_elements.ready->SetTop( 4 );
			m_elements.ready->SetTexture( g_engine->GetTextureLoader()->GetColorTexture( faction_color ) );
			m_elements.ready->SetStretchTexture( true );
			AddChild( m_elements.ready );
		}

		NEW( m_elements.faction, Dropdown, "PopupDropdown" );
		if ( is_it_me ) {
			m_elements.faction->SetChoices( m_parent->GetFactionChoices() );
		}
		m_elements.faction->SetValue( player->GetFaction().m_name );
		m_elements.faction->SetTextColor( faction_color );
		m_elements.faction->SetLeft( 218 );
		m_elements.faction->SetWidth( 140 );
		m_elements.faction->On(
			UIEvent::EV_CHANGE, EH( this, player, rules ) {
				player->SetFaction( rules.m_factions.at( data->value.change.id ) );
				m_parent->GetLobby()->UpdateSlot( m_slot_num, m_slot );
				return true;
			}
		);
		AddChild( m_elements.faction );

		NEW( m_elements.difficulty_level, Dropdown, "PopupDropdown" );
		if ( is_it_me ) {
			m_elements.difficulty_level->SetChoices( m_parent->GetDifficultyLevelChoices() );
		}
		m_elements.difficulty_level->SetValue( player->GetDifficultyLevel().m_name );
		m_elements.difficulty_level->SetTextColor( faction_color );
		m_elements.difficulty_level->SetLeft( 360 );
		m_elements.difficulty_level->SetWidth( 118 );
		m_elements.difficulty_level->On(
			UIEvent::EV_CHANGE, EH( this, player, rules ) {
				player->SetDifficultyLevel( rules.m_difficulty_levels.at( data->value.change.id ) );
				m_parent->GetLobby()->UpdateSlot( m_slot_num, m_slot );
				return true;
			}
		);
		AddChild( m_elements.difficulty_level );

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

		m_elements.actions->SetChoicesV( actions );
		m_elements.actions->On(
			UIEvent::EV_CHANGE, EH( this, is_it_me, am_i_host ) {
				const auto& value = *( data->value.change.text );
				if ( is_it_me ) {
					if ( value == "Customize faction" ) {
						g_engine->GetUI()->ShowError( "This feature is not available yet." );
						return true;
					}
				}
				else {
					if ( am_i_host ) {
						if ( value == "Kick" ) {
							m_parent->GetLobby()->KickFromSlot( m_slot_num );
							return true;
						}
						if ( value == "Ban" ) {
							m_parent->GetLobby()->BanFromSlot( m_slot_num );
							return true;
						}
					}
				}
				ASSERT( false, "invalid selection" );
				return false;
			}
		);
		m_elements.actions->SetValue( player->GetPlayerName() );
		m_elements.actions->SetTextColor( faction_color );

	}
	else {
		m_elements.actions->SetMode( Dropdown::DM_SELECT );

		if ( am_i_host ) {
			m_elements.actions->SetChoicesV(
				{
					"Open",
					"Closed"
				}
			);
		}
		if ( m_slot->GetState() == game::Slot::SS_OPEN ) {
			m_elements.actions->SetValue( "Open" );
		}
		else {
			m_elements.actions->SetValue( "Closed" );
		}
		m_elements.actions->On(
			UIEvent::EV_CHANGE, EH( this, am_i_host ) {
				const auto& value = *( data->value.change.text );
				if ( am_i_host ) {
					if ( value == "Open" ) {
						m_slot->Open();
						m_parent->GetLobby()->UpdateSlot( m_slot_num, m_slot );
						return true;
					}
					if ( value == "Closed" ) {
						m_slot->Close();
						m_parent->GetLobby()->UpdateSlot( m_slot_num, m_slot );
						return true;
					}
				}
				ASSERT( false, "invalid selection" );
				return false;
			}
		);
	}
	AddChild( m_elements.actions );

}

void PlayersSectionRow::Destroy() {

	if ( m_elements.ready ) {
		RemoveChild( m_elements.ready );
	}

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
