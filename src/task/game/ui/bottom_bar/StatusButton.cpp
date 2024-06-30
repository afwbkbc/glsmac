#include "StatusButton.h"

#include "task/game/Game.h"
#include "engine/Engine.h"
#include "task/game/ui/popup/EndTurnConfirmation.h"
#include "ui/object/Surface.h"
#include "ui/object/SoundEffect.h"
#include "loader/texture/TextureLoader.h"

namespace task {
namespace game {
namespace ui {

StatusButton::StatusButton( Game* game )
	: ::ui::object::Button( "BBTurnCompleteButton" )
	, m_game( game ) {
	On(
		::ui::event::EV_BUTTON_CLICK, EH( this ) {
			if ( m_is_clickable ) {
				switch ( m_status_type ) {
					case ::game::turn::TS_TURN_ACTIVE: {
						NEWV( popup, ui::popup::EndTurnConfirmation, m_game, UH( this ) {
							SetStatus( ::game::turn::TS_PLEASE_WAIT );
							m_game->CompleteTurn();
							m_flashing->Show();
						} );
						popup->Open();
						RemoveStyleModifier( ::ui::M_HOVER );
						break;
					}
					case ::game::turn::TS_TURN_COMPLETE: {
						SetStatus( ::game::turn::TS_PLEASE_WAIT );
						m_game->CompleteTurn();
						break;
					}
					case ::game::turn::TS_WAITING_FOR_PLAYERS: {
						SetStatus( ::game::turn::TS_PLEASE_WAIT );
						m_game->UncompleteTurn();
						break;
					}
					default: {
						THROW( "unexpected status type: " + std::to_string( m_status_type ) );
					}
				}
			}
			return true;
		}
	);
	On(
		::ui::event::EV_MOUSE_OVER, EH( this ) {
			if ( m_is_clickable ) {
				m_flashing->Hide();
			}
			return false;
		}
	);
	On(
		::ui::event::EV_MOUSE_OUT, EH( this ) {
			if ( m_is_clickable ) {
				m_flashing->Show();
			}
			return false;
		}
	);
}

void StatusButton::Create() {
	::ui::object::Button::Create();

	NEW( m_flashing, ::ui::object::Surface );
	m_flashing->SetMargin( 4 );
	m_flashing->SetTintAlpha( 0.0f );
	m_flashing->SetTexture( g_engine->GetTextureLoader()->LoadTexture( resource::PCX_CONSOLE_X2_A, 199, 113, 373, 129 ) );
	AddChild( m_flashing );

	NEW( m_sound, ::ui::object::SoundEffect, "BBTurnCompleteSound" );
	AddChild( m_sound );

	SetStatus( ::game::turn::TS_PLEASE_WAIT );
}

void StatusButton::Iterate() {
	::ui::object::Button::Iterate();

	while ( m_flash_timer.HasTicked() ) {
		if ( m_flash_reverse ) {
			m_flash_alpha -= m_flash_alpha_step;
			if ( m_flash_alpha <= 0.0f ) {
				m_flash_alpha = 0.0f;
				m_flash_reverse = false;
			}
		}
		else {
			m_flash_alpha += m_flash_alpha_step;
			if ( m_flash_alpha >= m_flash_alpha_target ) {
				m_flash_alpha = m_flash_alpha_target;
				m_flash_reverse = true;
			}
		}
		m_flashing->SetTintAlpha( m_flash_alpha );
	}
}

void StatusButton::Destroy() {
	RemoveChild( m_flashing );
	RemoveChild( m_sound );

	::ui::object::Button::Destroy();
}

static const std::unordered_map< ::game::turn::turn_status_t, std::string > s_status_labels = {
	{
		::game::turn::TS_PLEASE_WAIT,
		"PLEASE WAIT"
	},
	{
		::game::turn::TS_TURN_ACTIVE,
		"TURN COMPLETE",
	},
	{
		::game::turn::TS_TURN_COMPLETE,
		"TURN COMPLETE",
	},
	{
		::game::turn::TS_WAITING_FOR_PLAYERS,
		"WAITING FOR PLAYERS",
	}
};
void StatusButton::SetStatus( const ::game::turn::turn_status_t status ) {
	if ( m_status_type != status ) {
		m_status_type = status;
		ASSERT( s_status_labels.find( m_status_type ) != s_status_labels.end(), "invalid status type: " + std::to_string( m_status_type ) );
		SetLabel( s_status_labels.at( m_status_type ) );
		switch ( m_status_type ) {
			case ::game::turn::TS_TURN_COMPLETE: {
				StartFlashing( 1.0f, 1.0f );
				m_sound->Play();
				break;
			}
			case ::game::turn::TS_WAITING_FOR_PLAYERS: {
				StartFlashing( 0.25f, 0.25f );
				m_sound->Stop();
				break;
			}
			default: {
				StopFlashing();
				m_sound->Stop();
			}
		}
		m_is_clickable = m_status_type != ::game::turn::TS_PLEASE_WAIT;
		if ( m_is_clickable ) {
			UnblockEvents();
		}
		else {
			BlockEvents();
			RemoveStyleModifier( ::ui::M_HOVER );
		}
	}
}

void StatusButton::StartFlashing( const float alpha_per_sec, const float alpha_max ) {
	m_flash_alpha_target = alpha_max;
	m_flash_alpha_step = alpha_per_sec / 1000 * FLASH_INTERVAL_MS;
	if ( !m_flash_timer.IsRunning() ) {
		m_flash_alpha = 0.0f;
		m_flash_timer.SetInterval( FLASH_INTERVAL_MS );
	}
	else if ( m_flash_alpha > m_flash_alpha_target ) {
		m_flash_alpha = m_flash_alpha_target;
	}
}

void StatusButton::StopFlashing() {
	if ( m_flash_timer.IsRunning() ) {
		m_flash_timer.Stop();
		m_flashing->SetTintAlpha( 0.0f );
	}
}

}
}
}
