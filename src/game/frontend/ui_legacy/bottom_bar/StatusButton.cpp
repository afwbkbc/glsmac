#include "StatusButton.h"

#include "game/frontend/Game.h"
#include "engine/Engine.h"
#include "game/frontend/ui_legacy/popup/EndTurnConfirmation.h"
#include "ui_legacy/object/Surface.h"
#include "ui_legacy/object/SoundEffect.h"
#include "loader/texture/TextureLoader.h"

namespace game {
namespace frontend {
namespace ui_legacy {

StatusButton::StatusButton( Game* game )
	: ::ui_legacy::object::Button( "BBTurnCompleteButton" )
	, m_game( game ) {
	On(
		::ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			if ( m_is_clickable ) {
				switch ( m_status_type ) {
					case backend::turn::TS_TURN_ACTIVE: {
						NEWV( popup, ui_legacy::popup::EndTurnConfirmation, m_game, UH( this ) {
							SetStatus( backend::turn::TS_PLEASE_WAIT );
							m_game->CompleteTurn();
							m_flashing->Show();
						} );
						popup->Open();
						RemoveStyleModifier( ::ui_legacy::M_HOVER );
						break;
					}
					case backend::turn::TS_TURN_COMPLETE: {
						SetStatus( backend::turn::TS_PLEASE_WAIT );
						m_game->CompleteTurn();
						break;
					}
					case backend::turn::TS_WAITING_FOR_PLAYERS: {
						SetStatus( backend::turn::TS_PLEASE_WAIT );
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
		::ui_legacy::event::EV_MOUSE_OVER, EH( this ) {
			if ( m_is_clickable ) {
				m_flashing->Hide();
			}
			return false;
		}
	);
	On(
		::ui_legacy::event::EV_MOUSE_OUT, EH( this ) {
			if ( m_is_clickable ) {
				m_flashing->Show();
			}
			return false;
		}
	);
}

void StatusButton::Create() {
	::ui_legacy::object::Button::Create();

	NEW( m_flashing, ::ui_legacy::object::Surface );
	m_flashing->SetMargin( 4 );
	m_flashing->SetTintAlpha( 0.0f );
	m_flashing->SetTexture( g_engine->GetTextureLoader()->LoadTexture( resource::PCX_CONSOLE_X2_A, 199, 113, 373, 129 ) );
	AddChild( m_flashing );

	NEW( m_sound, ::ui_legacy::object::SoundEffect, "BBTurnCompleteSound" );
	AddChild( m_sound );

	SetStatus( backend::turn::TS_PLEASE_WAIT );
}

void StatusButton::Iterate() {
	::ui_legacy::object::Button::Iterate();

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

	::ui_legacy::object::Button::Destroy();
}

static const std::unordered_map< backend::turn::turn_status_t, std::string > s_status_labels = {
	{
		backend::turn::TS_PLEASE_WAIT,
		"PLEASE WAIT"
	},
	{
		backend::turn::TS_TURN_ACTIVE,
		"TURN COMPLETE",
	},
	{
		backend::turn::TS_TURN_COMPLETE,
		"TURN COMPLETE",
	},
	{
		backend::turn::TS_WAITING_FOR_PLAYERS,
		"WAITING FOR PLAYERS",
	}
};
void StatusButton::SetStatus( const backend::turn::turn_status_t status ) {
	if ( m_status_type != status ) {
		m_status_type = status;
		ASSERT( s_status_labels.find( m_status_type ) != s_status_labels.end(), "invalid status type: " + std::to_string( m_status_type ) );
		SetLabel( s_status_labels.at( m_status_type ) );
		switch ( m_status_type ) {
			case backend::turn::TS_TURN_COMPLETE: {
				StartFlashing( 1.0f, 1.0f );
				m_sound->Play();
				break;
			}
			case backend::turn::TS_WAITING_FOR_PLAYERS: {
				StartFlashing( 0.25f, 0.25f );
				m_sound->Stop();
				break;
			}
			default: {
				StopFlashing();
				m_sound->Stop();
			}
		}
		m_is_clickable = m_status_type != backend::turn::TS_PLEASE_WAIT;
		if ( m_is_clickable ) {
			UnblockEvents();
		}
		else {
			BlockEvents();
			RemoveStyleModifier( ::ui_legacy::M_HOVER );
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
