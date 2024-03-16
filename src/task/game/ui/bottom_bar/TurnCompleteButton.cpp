#include "TurnCompleteButton.h"

#include "task/game/Game.h"
#include "engine/Engine.h"

namespace task {
namespace game {
namespace ui {

TurnCompleteButton::TurnCompleteButton( Game* game )
	: ::ui::object::Button( "BBTurnCompleteButton" )
	, m_game( game ) {
	On(
		UIEvent::EV_BUTTON_CLICK, EH( this ) {
			if ( m_is_turn_active ) {
				m_game->CompleteTurn();
			}
			else {
				m_game->UncompleteTurn();
			}
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_OVER, EH( this ) {
			if ( m_is_turn_active ) {
				m_flashing->Hide();
			}
			return false;
		}
	);
	On(
		UIEvent::EV_MOUSE_OUT, EH( this ) {
			if ( m_is_turn_active ) {
				m_flashing->Show();
			}
			return false;
		}
	);
}

void TurnCompleteButton::Create() {
	::ui::object::Button::Create();

	NEW( m_flashing, ::ui::object::Surface );
	m_flashing->SetMargin( 4 );
	m_flashing->SetTintAlpha( 0.0f );
	m_flashing->SetTexture( g_engine->GetTextureLoader()->LoadTexture( "console_x2_a.pcx", 199, 113, 373, 129 ) );
	AddChild( m_flashing );

	NEW( m_sound, SoundEffect, "BBTurnCompleteSound" );
	AddChild( m_sound );

	m_is_turn_active = true;
	SetTurnActiveStatus( false );
}

void TurnCompleteButton::Iterate() {
	::ui::object::Button::Iterate();

	const auto step = m_is_turn_active
		? FLASH_ALPHA_STEP_ACTIVE
		: FLASH_ALPHA_STEP_INACTIVE;
	const auto max = m_is_turn_active
		? FLASH_ALPHA_MAX_ACTIVE
		: FLASH_ALPHA_MAX_INACTIVE;

	while ( m_flash_timer.HasTicked() ) {
		if ( m_flash_reverse ) {
			m_flash_alpha -= step;
			if ( m_flash_alpha <= FLASH_ALPHA_MIN ) {
				m_flash_alpha = FLASH_ALPHA_MIN;
				m_flash_reverse = false;
			}
		}
		else {
			m_flash_alpha += step;
			if ( m_flash_alpha >= max ) {
				m_flash_alpha = max;
				m_flash_reverse = true;
			}
		}
		m_flashing->SetTintAlpha( m_flash_alpha );
	}
}

void TurnCompleteButton::Destroy() {
	RemoveChild( m_flashing );
	RemoveChild( m_sound );

	::ui::object::Button::Destroy();
}

void TurnCompleteButton::SetTurnActiveStatus( const bool is_turn_active ) {
	if ( m_is_turn_active != is_turn_active ) {
		m_is_turn_active = is_turn_active;
		if ( is_turn_active ) {
			SetFlashingEnabled( false, false );
			SetLabel( "TURN COMPLETE" );
		}
		else {
			SetLabel( "WAITING FOR PLAYERS" );
			m_flash_alpha = FLASH_ALPHA_MIN;
			SetFlashingEnabled( true, false );
			m_sound->Stop();
			m_flashing->Show();
		}
	}
}

void TurnCompleteButton::SetFlashingEnabled( const bool is_flashing_enabled, const bool play_sound ) {
	if ( is_flashing_enabled && !m_flash_timer.IsRunning() ) {
		m_flash_alpha = FLASH_ALPHA_MIN;
		m_flash_timer.SetInterval( FLASH_INTERVAL_MS );
		if ( play_sound ) {
			m_sound->Play();
		}
	}
	else if ( !is_flashing_enabled && m_flash_timer.IsRunning() ) {
		m_flash_timer.Stop();
		m_flashing->SetTintAlpha( 0.0f );
		m_sound->Stop();
	}
}

}
}
}
