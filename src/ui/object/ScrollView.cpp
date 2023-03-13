#include "ScrollView.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

ScrollView::ScrollView( const std::string& class_name )
	: Panel( class_name )
{
	
}

void ScrollView::Create() {
	Panel::Create();
	
	NEW( m_viewport, UIContainer );
		m_viewport->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_viewport->SetMargin( m_border_size );
		//m_viewport->SetOverflow( UIObject::OVERFLOW_HIDDEN ); // TODO: fix so that it works here
	Panel::AddChild( m_viewport );
	
	NEW( m_body, Panel );
		m_body->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_body->SetLeft( -m_scroll.x );
		m_body->SetTop( -m_scroll.y );
		m_body->SetWidth( m_internal_size.x );
		m_body->SetHeight( m_internal_size.y );
		m_body->SetOverflow( UIObject::OVERFLOW_GROW );
	m_viewport->AddChild( m_body );
	
	SetScroll( m_scroll );
	
	if ( !m_to_add.empty() ) {
		for ( auto& child : m_to_add ) {
			child->SetOverflowMargin( m_border_size );
			m_body->AddChild( child );
		}
		m_to_add.clear();
	}
	
	m_viewport->SetOverflow( UIObject::OVERFLOW_HIDDEN );
	
	// TODO: make dragging global event
	On( UIEvent::EV_MOUSE_DOWN, EH( this ) {
		if ( data->mouse.button == UIEvent::M_RIGHT ) {
			m_is_dragging = true;
			m_drag_start_position = {
				data->mouse.absolute.x + (ssize_t)m_scroll.x,
				data->mouse.absolute.y + (ssize_t)m_scroll.y
			};
		}
		return true;
	});

	On( UIEvent::EV_MOUSE_SCROLL, EH( this ) {
		SetScrollY( m_scroll.y + (coord_t) data->mouse.scroll_y * m_scroll_speed );
		return true;
	});

	auto* ui = g_engine->GetUI();
	// mousemove should keep working if mouse is outside minimap but dragging continues
	m_handlers.mouse_move = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_MOVE, EH( this ) {
		if ( m_is_dragging ) {
			SetScrollX( m_drag_start_position.x - data->mouse.absolute.x );
			SetScrollY( m_drag_start_position.y - data->mouse.absolute.y );
		}
		return false;
	}, ::ui::UI::GH_BEFORE );

	// dragging should be cancelable anywhere
	m_handlers.mouse_up = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
		if ( data->mouse.button == UIEvent::M_RIGHT ) {
			if ( m_is_dragging ) {
				m_is_dragging = false;
			}
		}
		return false;
	}, ::ui::UI::GH_BEFORE );
	
}

void ScrollView::Destroy() {
	
	auto* ui = g_engine->GetUI();
	ui->RemoveGlobalEventHandler( m_handlers.mouse_move );
	ui->RemoveGlobalEventHandler( m_handlers.mouse_up );
	
	if ( !m_to_remove.empty() ) {
		for ( auto& child : m_to_remove ) {
			m_body->RemoveChild( child );
		}
		m_to_remove.clear();
	}
	m_viewport->RemoveChild( m_body );
	Panel::RemoveChild( m_viewport );
	
	Panel::Destroy();
}

void ScrollView::AddChild( UIObject* object ) {
	if ( m_body ) {
		object->SetOverflowMargin( m_border_size );
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

void ScrollView::SetScroll( vertex_t px ) {
	if ( m_scroll != px ) {
		if ( m_body ) {
			m_scroll = px;
			const auto& area = GetObjectArea();
			const auto& body_area = m_body->GetInternalObjectArea();
			const coord_box_t limits = {
				0, // left
				0, // top
				body_area.width - area.width + m_border_size * 2, // right
				body_area.height - area.height + m_border_size * 2 // bottom
			};
			if ( m_scroll.x > limits.right ) {
				m_scroll.x = limits.right;
			}
			if ( m_scroll.x < limits.left ) {
				m_scroll.x = limits.left;
			}
			if ( m_scroll.y > limits.bottom ) {
				m_scroll.y = limits.bottom;
			}
			if ( m_scroll.y < limits.top ) {
				m_scroll.y = limits.top;
			}
			Log( "Scrolling to " + m_scroll.ToString() + " (area: " + std::to_string( area.left ) + " " + std::to_string( area.top ) + " " + std::to_string( area.right ) + " " + std::to_string( area.bottom ) + " " + std::to_string( area.width ) + " " + std::to_string( area.height ) + " )" );
			m_body->SetLeft( -m_scroll.x );
			m_body->SetTop( -m_scroll.y );
		}
	}
}

void ScrollView::SetScrollX( const coord_t px ) {
	SetScroll({ px, m_scroll.y });
}

void ScrollView::SetScrollY( const coord_t px ) {
	SetScroll({ m_scroll.x, px });
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
