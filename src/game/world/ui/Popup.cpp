#include "Popup.h"

#include "../World.h"
#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {

Popup::Popup( World* world )
	: ::ui::object::Popup( "WP" )
	, m_world( world )
{
	//
}

void Popup::Create() {
	::ui::object::Popup::Create();
	
	NEW( m_body, Section, m_world, "", "WP" );
	AddChild( m_body );
	 
	#define x( _k, _n ) {\
		NEW( m_side_frames._k, ::ui::object::Surface, "WP" _n "Frame" ); \
		AddChild( m_side_frames._k ); \
	}
		x( left_left, "LeftLeft" );
		x( left_right, "LeftRight" );
		x( right_right, "RightRight" );
		x( right_left, "RightLeft" );
	#undef x

	auto *soundloader = g_engine->GetSoundLoader();
		
	NEW( m_sounds.slideup, SoundEffect );
		m_sounds.slideup->SetSound( soundloader->LoadSound( "menu up.wav" ) );
	AddChild( m_sounds.slideup );
	NEW( m_sounds.slidedown, SoundEffect );
		m_sounds.slidedown->SetSound( soundloader->LoadSound( "menu down.wav" ) );
	AddChild( m_sounds.slidedown );
	
	// slide up
	const auto start_at = m_world->GetBottomBarMiddleHeight() - GetHeight();
	SetBottom( start_at );
	m_slide.Scroll( start_at, start_at + GetHeight() );	
	m_sounds.slideup->Play();
}

void Popup::Iterate() {
	::ui::object::Popup::Iterate();
	
	bool has_ticked = false;
	while ( m_slide.HasTicked() ) {
		has_ticked = true;
		SetBottom( m_slide.GetPosition() );
	}
	if ( !has_ticked && m_is_closing ) {
		g_engine->GetUI()->ClosePopup( this );
	}
}

void Popup::Destroy() {
	RemoveChild( m_body );
	
	RemoveChild( m_sounds.slideup );
	RemoveChild( m_sounds.slidedown );
	
	#define x( _k ) RemoveChild( m_side_frames._k )
		x( left_left );
		x( left_right );
		x( right_right );
		x( right_left );
	#undef x
	
	::ui::object::Popup::Destroy();
}

void Popup::ProcessEvent( event::UIEvent* event ) {
	if ( !m_slide.IsRunning() ) { // ignore events during slide
		::ui::object::Popup::ProcessEvent( event );
	}
}

bool Popup::MaybeClose() {
	if ( m_is_closing ) {
		return !m_slide.IsRunning(); // ready to close if slide down finished
	}
	else {
		// slide down
		m_is_closing = true;
		m_slide.Scroll( GetBottom(), m_world->GetBottomBarMiddleHeight() - GetHeight() );
		m_sounds.slidedown->Play();
		return false;
	}
}

}
}
}
