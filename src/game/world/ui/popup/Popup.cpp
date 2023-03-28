#include "Popup.h"

#include "../../World.h"
#include "engine/Engine.h"

namespace game {
namespace world {
namespace ui {
namespace popup {

Popup::Popup( World* world )
	: object::Popup( "WP" )
	, m_world( world )
{
	//
}

void Popup::Create() {
	object::Popup::Create();
	
	NEW( m_body, Section, m_world, "", "WP" );
	if ( !m_title_text.empty() ) {
		m_body->SetTitleText( m_title_text );
	}
	AddChild( m_body );
	
	#define x( _k, _n ) {\
		NEW( m_side_frames._k, object::Surface, "WP" _n "Frame" ); \
		AddChild( m_side_frames._k ); \
	}
		x( left_left, "LeftLeft" );
		x( left_right, "LeftRight" );
		x( right_right, "RightRight" );
		x( right_left, "RightLeft" );
	#undef x

	// slide up
	const auto start_at = m_world->GetBottomBarMiddleHeight() - GetHeight();
	SetBottom( start_at );
	m_slide.Scroll( start_at, start_at + GetHeight() );
}

void Popup::Align() {
	
	// don't let popup top go outside window
	// TODO: add 'max_height' to styles
	object::Popup::SetHeight( std::min< coord_t >( m_original_height, m_world->GetViewportHeight() ) );

	object::Popup::Align();
}

void Popup::Iterate() {
	object::Popup::Iterate();
	
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
	
	#define x( _k ) RemoveChild( m_side_frames._k )
		x( left_left );
		x( left_right );
		x( right_right );
		x( right_left );
	#undef x
	
	object::Popup::Destroy();
}

void Popup::ProcessEvent( event::UIEvent* event ) {
	if ( !m_slide.IsRunning() ) { // ignore events during slide
		object::Popup::ProcessEvent( event );
	}
}

bool Popup::MaybeClose() {
	if ( m_is_closing ) {
		if ( !m_slide.IsRunning() ) { // ready to close if slide down finished
			return object::Popup::MaybeClose();
		}
	}
	else {
		// slide down
		m_is_closing = true;
		m_slide.Scroll( GetBottom(), m_world->GetBottomBarMiddleHeight() - GetHeight() );
		PlayCloseSound();
	}
	return false; // not ready to close yet
}

void Popup::SetHeight( const coord_t px ) {
	m_original_height = px;
	object::Popup::SetHeight( px );
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
