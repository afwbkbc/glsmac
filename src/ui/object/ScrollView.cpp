#include "ScrollView.h"

namespace ui {
namespace object {

ScrollView::ScrollView( const std::string& class_name )
	: Panel( class_name )
{
	
}

void ScrollView::Create() {
	Panel::Create();
	
	NEW( m_body, Panel );
		m_body->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_body->SetLeft( 0 );
		m_body->SetTop( 0 );
		m_body->SetOverflow( UIObject::OVERFLOW_GROW );
	Panel::AddChild( m_body );
	
	UpdateInternalSize();
	
	if ( !m_to_add.empty() ) {
		for ( auto& child : m_to_add ) {
			m_body->AddChild( child );
		}
		m_to_add.clear();
	}
	
	SetOverflow( OVERFLOW_HIDDEN );
}

void ScrollView::Destroy() {
	if ( !m_to_remove.empty() ) {
		for ( auto& child : m_to_remove ) {
			m_body->RemoveChild( child );
		}
		m_to_remove.clear();
	}
	Panel::RemoveChild( m_body );
	
	Panel::Destroy();
}

void ScrollView::AddChild( UIObject* object ) {
	if ( m_body ) {
		m_body->AddChild( object );
	}
	else {
		m_to_add.push_back( object );
	}
}

void ScrollView::RemoveChild( UIObject* object ) {
	if ( m_body ) {
		m_body->RemoveChild( object );
	}
	else {
		m_to_remove.push_back( object );
	}
}

void ScrollView::SetWidth( const coord_t px ) {
	Panel::SetWidth( px );
	
	if ( m_internal_size.x < px ) {
		SetInternalWidth( px );
	}
}

void ScrollView::SetHeight( const coord_t px ) {
	Panel::SetHeight( px );
	
	if ( m_internal_size.y < px ) {
		SetInternalHeight( px );
	}
}

void ScrollView::SetInternalWidth( const coord_t px ) {
	if ( m_internal_size.x != px ) {
		m_internal_size.x = std::max< coord_t >( px, GetWidth() );
		UpdateInternalSize();
	}
}

void ScrollView::SetInternalHeight( const coord_t px ) {
	if ( m_internal_size.y != px ) {
		m_internal_size.y = std::max< coord_t >( px, GetHeight() );
		UpdateInternalSize();
	}
}

void ScrollView::UpdateInternalSize() {
	if ( m_body ) {
		if ( m_body->GetWidth() != m_internal_size.x ) {
			m_body->SetWidth( m_internal_size.x );
		}
		if ( m_body->GetHeight() != m_internal_size.y ) {
			m_body->SetHeight( m_internal_size.y );
		}
	}
}

}
}
