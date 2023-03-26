#include "Popup.h"

#include "../../World.h"
#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

Popup::Popup( World* world )
	: ::ui::object::Popup( "WP" )
	, m_world( world )
{
	//
}

void Popup::Create() {
	::ui::object::Popup::Create();
	
	NEW( m_body, Section, m_world, "", "WP" );
	if ( !m_title_text.empty() ) {
		m_body->SetTitleText( m_title_text );
	}
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
		m_sounds.slideup->SetVolume( 0.5f );
	AddChild( m_sounds.slideup );
	NEW( m_sounds.slidedown, SoundEffect );
		m_sounds.slidedown->SetSound( soundloader->LoadSound( "menu down.wav" ) );
		m_sounds.slideup->SetVolume( 0.7f );
	AddChild( m_sounds.slidedown );
	
	// slide up
	const auto start_at = m_world->GetBottomBarMiddleHeight() - GetHeight();
	SetBottom( start_at );
	m_slide.Scroll( start_at, start_at + GetHeight() );
	
	m_sounds.slideup->Play();
}

void Popup::Align() {
	
	// don't let popup top go outside window
	// TODO: add 'max_height' to styles
	::ui::object::Popup::SetHeight( std::min< coord_t >( m_original_height, m_world->GetViewportHeight() ) );

	// TODO: make proper z index logic for texts
	const auto* g = g_engine->GetGraphics();
	SetAreaLimits({
		0,
		0,
		(coord_t) g->GetViewportWidth(),
		(coord_t) m_world->GetViewportHeight()
	});
	
	::ui::object::Popup::Align();
}

void Popup::Iterate() {
	::ui::object::Popup::Iterate();
	
	bool has_ticked = false;
	while ( m_slide.HasTicked() ) {
		has_ticked = true;
		SetBottom( m_slide.GetPosition() );
	}
	if ( !has_ticked && m_is_closing ) {
		// slide down finished
		Close();
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
		if ( !m_slide.IsRunning() ) { // ready to close if slide down finished
			return ::ui::object::Popup::MaybeClose();
		}
	}
	else {
		// slide down
		m_is_closing = true;
		m_slide.Scroll( GetBottom(), m_world->GetBottomBarMiddleHeight() - GetHeight() );
		
		m_sounds.slidedown->Play();
	}
	return false; // not ready to close yet
}

void Popup::SetHeight( const coord_t px ) {
	m_original_height = px;
	::ui::object::Popup::SetHeight( px );
}

void Popup::SetTitleText( const std::string& title_text ) {
	if ( m_title_text != title_text ) {
		m_title_text = title_text;
		if ( m_body ) {
			m_body->SetTitleText( m_title_text );
		}
	}
}

}
}
}
}
