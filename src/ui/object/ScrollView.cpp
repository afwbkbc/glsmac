#include "ScrollView.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

void ScrollView::Create() {
	Panel::Create();
	
	NEW( m_viewport, UIContainer );
		m_viewport->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_viewport->SetMargin( m_border_size );
		m_viewport->SetRight( 20 ); // to leave space for scrollbar // TODO: put to style
		m_viewport->SetOverflow( UIObject::OVERFLOW_HIDDEN );
	Panel::AddChild( m_viewport );
	
	NEW( m_body, Panel );
		m_body->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_body->SetWidth( m_internal_size.x );
		m_body->SetHeight( m_internal_size.y );
		m_body->SetMargin( m_border_size );
		m_body->SetOverflow( UIObject::OVERFLOW_GROW );
		m_body->On( UIEvent::EV_RESIZE, EH( this ) {
			m_need_body_refresh = true;
			return true;
		});
	m_viewport->AddChild( m_body );
	
	const auto f_handle_mouse_scroll = [ this ]( const UIEvent::event_data_t* data ) -> void {
		ScrollTo( m_scroll.y - (coord_t)data->mouse.scroll_y * m_scroll_speed );
		if ( m_is_sticky ) {
			m_need_stickyness = true;
		}
	};
	
	NEW( m_scrollbar, Scrollbar, Scrollbar::SD_VERTICAL, "ScrollbarVerticalThick" );
		m_scrollbar->SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_VCENTER );
		m_scrollbar->SetTop( m_border_size + 1 );
		m_scrollbar->SetRight( m_border_size + 1 );
		m_scrollbar->SetBottom( m_border_size + 1 );
		m_scrollbar->On( UIEvent::EV_MOUSE_SCROLL, EH( this, f_handle_mouse_scroll ) {
			f_handle_mouse_scroll( data );
			return true;
		});
		m_scrollbar->On( UIEvent::EV_CHANGE, EH( this ) {
			if ( data->value.scroll.is_scrolling ) {
				const auto limits = GetScrollLimits();
				SetScrollY( ( limits.bottom - limits.top )  * data->value.scroll.percentage / 100.0f );
			}
			else if ( m_is_sticky ) {
				m_need_stickyness = true;
			}
			return true;
		});
		m_scrollbar->SetZIndex( 0.7f ); // TODO: fix z index bugs
	Panel::AddChild( m_scrollbar );
	
	if ( !m_to_add.empty() ) {
		for ( auto& child : m_to_add ) {
			AddChildToBody( child );
		}
		m_to_add.clear();
	}
	
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

	On( UIEvent::EV_MOUSE_SCROLL, EH( this, f_handle_mouse_scroll ) {
		f_handle_mouse_scroll( data );
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
				if ( m_is_sticky ) {
					m_scroller.Scroll( m_scroll.y, (ssize_t)round( m_scroll.y + m_scroll_speed / 2.0f ) / (ssize_t)m_scroll_speed * (ssize_t)m_scroll_speed );
					// TODO: fix strange 'bounce' at bottom
				}
			}
		}
		return false;
	}, ::ui::UI::GH_BEFORE );

	m_need_body_refresh = true;
}

void ScrollView::Iterate() {
	Panel::Iterate();
	
	if ( m_need_body_refresh ) {
		//Log( "Refreshing body" );
		m_need_body_refresh = false;
		m_scroller.Stop();
		SetScroll( m_scroll, true );
	}

	
	if ( !m_scroller.IsRunning() && m_need_stickyness ) {
		m_need_stickyness = false;
		m_scroller.Scroll(
			m_scroll.y,
			(ssize_t) round( m_scroll.y + m_scroll_speed / 2.0f )
				/
			(ssize_t) m_scroll_speed * (ssize_t) m_scroll_speed
		);
	}
	
	while ( m_scroller.HasTicked() ) {
		SetScrollY( m_scroller.GetPosition() );
	}
}

void ScrollView::Destroy() {
	
	auto* ui = g_engine->GetUI();
	ui->RemoveGlobalEventHandler( m_handlers.mouse_move );
	ui->RemoveGlobalEventHandler( m_handlers.mouse_up );
	
	if ( !m_to_remove.empty() ) {
		for ( auto& child : m_to_remove ) {
			RemoveChildFromBody( child );
		}
		m_to_remove.clear();
	}
	
	Panel::RemoveChild( m_scrollbar );
	
	m_viewport->RemoveChild( m_body );
	Panel::RemoveChild( m_viewport );
	
	Panel::Destroy();
}

void ScrollView::AddChild( UIObject* object ) {
	if ( m_body ) {
		AddChildToBody( object );
	}
	else {
		m_to_add.push_back( object );
	}
}

void ScrollView::RemoveChild( UIObject* object ) {
	if ( m_body ) {
		RemoveChildFromBody( object );
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

void ScrollView::SetScroll( vertex_t px, const bool force ) {
	if ( m_scroll != px || force ) {
		m_scroll = px;
		const coord_box_t limits = GetScrollLimits();
		if ( m_body ) {
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
			//Log( "Scrolling to " + m_scroll.ToString() + " (area: " + std::to_string( area.left ) + " " + std::to_string( area.top ) + " " + std::to_string( area.right ) + " " + std::to_string( area.bottom ) + " " + std::to_string( area.width ) + " " + std::to_string( area.height ) + " )" );
			m_body->SetLeft( -m_scroll.x );
			m_body->SetTop( -m_scroll.y );
			
		}
		if ( m_scrollbar ) {
			const auto& area = GetInternalObjectArea();
			const auto& body_area = m_body->GetObjectArea();
			if ( body_area.height > area.height ) {
				const float p = (float)m_scroll.y / ( limits.bottom - limits.top ) * 100.0f;
				//Log( "Setting scrollbar to " + std::to_string( p ) );
				m_scrollbar->SetPercentage( p );
				m_scrollbar->Show();
			}
			else {
				m_scrollbar->Hide();
			}
		}
	}
}

void ScrollView::SetScrollX( const coord_t px ) {
	SetScroll({ px, m_scroll.y });
}

void ScrollView::SetScrollY( const coord_t px ) {
	SetScroll({ m_scroll.x, px });
}

void ScrollView::ScrollToBottom() {
	const auto limits = GetScrollLimits();
	SetScrollY( ( limits.bottom - limits.top ) );
}

void ScrollView::SetScrollSpeed( const size_t scroll_speed ) {
	m_scroll_speed = scroll_speed;
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

void ScrollView::ScrollToObjectMaybe( UIObject* object ) {
	if ( object->GetTop() < m_scroll.y ) {
		SetScrollY( object->GetTop() );
	}
	else {
		const auto area = GetInternalObjectArea();
		const auto& object_area = object->GetObjectArea();
		const auto h = m_scroll.y + area.height;
		const auto oh = object->GetTop() + object_area.height + 2; // TODO: why +2 ?
		if ( oh > h ) {
			SetScrollY( oh - area.height );
		}
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

const UIObject::coord_box_t ScrollView::GetScrollLimits() {
	const auto& area = GetInternalObjectArea();
	const auto& body_area = m_body->GetObjectArea();
	return {
		0, // left
		0, // top
		body_area.width - area.width + m_border_size * 2, // right
		body_area.height - area.height + m_border_size * 2 + 2 // bottom // TODO: why + 2 ?
	};
}

void ScrollView::ScrollTo( float target ) {
	const float source = m_scroller.IsRunning() ? m_scroller.GetTargetPosition() : m_scroll.y;
	m_scroller.Scroll( source, target );
};

void ScrollView::AddChildToBody( UIObject* child ) {
	//child->SetOverflowMargin( m_border_size );
	child->SetParentStyleObject( this );
	m_body->AddChild( child );
	FixScrollY();
}

void ScrollView::RemoveChildFromBody( UIObject* child ) {
	m_body->RemoveChild( child );
	FixScrollY();
}

void ScrollView::FixScrollY() {
	const auto limits = GetScrollLimits();
	float scroll_y = ( limits.bottom - limits.top ) * m_scrollbar->GetPercentage() / 100.0f;
	//if ( m_is_sticky ) {
		//scroll_y = (ssize_t)round( scroll_y ) / (ssize_t)m_scroll_speed * (ssize_t)m_scroll_speed;
	//}
	SetScrollY( scroll_y );
}

}
}
