#include "TurnCompleteButton.h"

#include "engine/Engine.h"

namespace task {
namespace game {
namespace ui {

TurnCompleteButton::TurnCompleteButton()
	: ::ui::object::Button( "BBTurnCompleteButton" ) {
	SetLabel( "TURN COMPLETE" );
	On(
		UIEvent::EV_BUTTON_CLICK, EH( this ) {
			Log( "TURN COMPLETE" );
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_OVER, EH( this ) {
			m_flashing->Hide();
			return true;
		}
	);
	On(
		UIEvent::EV_MOUSE_OUT, EH( this ) {
			m_flashing->Show();
			return true;
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
}

void TurnCompleteButton::Iterate() {
	::ui::object::Button::Iterate();

	while ( m_flash_timer.HasTicked() ) {
		if ( m_flash_reverse ) {
			m_flash_alpha -= FLASH_ALPHA_STEP;
			if ( m_flash_alpha <= FLASH_ALPHA_MIN ) {
				m_flash_alpha = FLASH_ALPHA_MIN;
				m_flash_reverse = false;
			}
		}
		else {
			m_flash_alpha += FLASH_ALPHA_STEP;
			if ( m_flash_alpha >= FLASH_ALPHA_MAX ) {
				m_flash_alpha = FLASH_ALPHA_MAX;
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

void TurnCompleteButton::SetTurnCompleteStatus( const bool is_turn_complete ) {
	if ( is_turn_complete && !m_flash_timer.IsRunning() ) {
		m_flash_timer.SetInterval( FLASH_INTERVAL_MS );
		m_sound->Play();
	}
	else if ( !is_turn_complete && m_flash_timer.IsRunning() ) {
		m_flash_timer.Stop();
		m_flashing->SetTintAlpha( 0.0f );
	}
}

}
}
}
