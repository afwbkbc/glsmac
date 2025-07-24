#include "Scrollview.h"

#include <cmath>

#include "Scrollbar.h"

#include "input/Event.h"
#include "ui/UI.h"
#include "ui/geometry/Geometry.h"
#include "gse/value/Bool.h"

namespace ui {
namespace dom {

Scrollview::Scrollview( DOM_ARGS_T, const bool factories_allowed )
	: Panel( DOM_ARGS_PASS_T, factories_allowed ) {

	Events(
		{
			//input::EV_MOUSE_SCROLL // TODO
		}
	);

	m_inner = new Container( GSE_CALL, ui, this, {}, "inner", false );
	{
		auto* g = m_inner->GetGeometry();
		g->SetWidth( 0 );
		g->SetHeight( 0 );
		g->SetOverflowMode( geometry::Geometry::OM_RESIZE );
		g->m_on_resize = [ this ]( const size_t width, const size_t height ) {
			if ( !m_is_updating ) {
				m_is_updating = true;
				UpdateScrollbars( width, height );
				m_is_updating = false;
			}
		};
	}
	Embed( m_inner );
	m_factory_owner = m_inner;

	m_geometry->SetOverflowMode( geometry::Geometry::OM_HIDDEN );

	m_vscroll = new Scrollbar( GSE_CALL, ui, this, {} );
	{
		auto* g = m_vscroll->GetGeometry();
		g->SetAlign( geometry::Geometry::ALIGN_RIGHT_CENTER );
		g->SetTop( 0 );
		g->SetBottom( 0 );
		g->SetRight( 0 );
	}
	m_vscroll->m_on_change = [ this ]( const float value ) {
		m_inner->GetGeometry()->SetTop( -value + m_padding );
		return true;
	};
	Embed( m_vscroll, false );
	ForwardProperty( GSE_CALL, "vscroll_class", "class", m_vscroll );

	m_hscroll = new Scrollbar( GSE_CALL, ui, this, {} );
	{
		auto* g = m_hscroll->GetGeometry();
		g->SetAlign( geometry::Geometry::ALIGN_BOTTOM_CENTER );
		g->SetLeft( 0 );
		g->SetRight( 0 );
		g->SetBottom( 0 );
	}
	m_hscroll->m_on_change = [ this ]( const float value ) {
		m_inner->GetGeometry()->SetLeft( -value + m_padding );
		return true;
	};
	Embed( m_hscroll, false );
	ForwardProperty( GSE_CALL, "hscroll_class", "class", m_hscroll );

	PROPERTY( "padding", gse::value::Int, 0, SetPadding );
	PROPERTY( "has_vscroll", gse::value::Bool, true, SetHasVScroll );
	PROPERTY( "has_hscroll", gse::value::Bool, true, SetHasHScroll );
	PROPERTY( "autoscroll", gse::value::Bool, 0, SetAutoScroll );

	m_drag.handler = [ this ]( GSE_CALLABLE, const input::Event& event ) {
		if ( m_drag.is_dragging ) {
			switch ( event.type ) {
				case input::EV_MOUSE_MOVE: {
					if ( m_hscroll->m_is_visible ) {
						m_hscroll->SetValue( GSE_CALL, std::fmax( m_hscroll->m_min, std::fmin( m_hscroll->m_max, m_drag.initial_offset_x - event.data.mouse.x ) ), true );
					}
					if ( m_vscroll->m_is_visible ) {
						m_vscroll->SetValue( GSE_CALL, std::fmax( m_vscroll->m_min, std::fmin( m_vscroll->m_max, m_drag.initial_offset_y - event.data.mouse.y ) ), true );
					}
					return true;
				}
				case input::EV_MOUSE_UP: {
					m_drag.is_dragging = false;
					return false;
				}
				case input::EV_MOUSE_SCROLL: {
					return true;
				};
			}
		}
		return false;
	};

	UpdateScrollbars( 0, 0 );
}

const bool Scrollview::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	if ( event.flags & input::EF_MOUSE && event.data.mouse.button == input::MB_MIDDLE ) {
		switch ( event.type ) {
			case input::EV_MOUSE_DOWN: {
				m_drag.initial_offset_x = event.data.mouse.x + m_hscroll->m_value;
				m_drag.initial_offset_y = event.data.mouse.y + m_vscroll->m_value;
				m_drag.is_dragging = true;
				m_drag.drag_handler_id = m_ui->AddGlobalHandler( m_drag.handler );
				return true;
			}
			case input::EV_MOUSE_UP: {
				if ( m_drag.is_dragging ) {
					ASSERT( m_drag.drag_handler_id, "drag handler id zero" );
					m_ui->RemoveGlobalHandler( m_drag.drag_handler_id );
					m_drag.drag_handler_id = 0;
					m_drag.is_dragging = false;
				}
				return false;
			}
		}
	}
	if ( event.type == input::EV_MOUSE_SCROLL && !m_drag.is_dragging ) {
		if ( m_vscroll->m_is_visible ) {
			return m_vscroll->ProcessEvent( GSE_CALL, event );
		}
		else if ( m_hscroll->m_is_visible ) {
			return m_hscroll->ProcessEvent( GSE_CALL, event );
		}
	}
	return Panel::ProcessEvent( GSE_CALL, event );
}

void Scrollview::SetHasVScroll( GSE_CALLABLE, const bool value ) {
	if ( value != m_has_vscroll ) {
		m_has_vscroll = value;
		auto* g = m_inner->GetGeometry();
		UpdateScrollbars( g->m_area.width, g->m_area.height );
	}
}

void Scrollview::SetHasHScroll( GSE_CALLABLE, const bool value ) {
	if ( value != m_has_hscroll ) {
		m_has_hscroll = value;
		auto* g = m_inner->GetGeometry();
		UpdateScrollbars( g->m_area.width, g->m_area.height );
	}
}

void Scrollview::UpdateScrollbars( const size_t width, const size_t height ) {
	const auto vscroll_w = m_vscroll->GetGeometry()->m_area.width;
	const auto hscroll_h = m_hscroll->GetGeometry()->m_area.height;
	auto outer_w = m_geometry->m_area.width;
	auto outer_h = m_geometry->m_area.height;
	auto* inner_g = m_inner->GetGeometry();
	float needscroll = false;

	const bool need_v = m_has_vscroll && height > outer_h - m_padding * 2;
	const bool need_h = m_has_hscroll && width > outer_w - m_padding * 2;

	if ( need_v ) {
		outer_w -= vscroll_w;
		inner_g->SetWidth( outer_w );
	}
	if ( need_h ) {
		outer_h -= hscroll_h;
		inner_g->SetHeight( outer_h );
	}

	if ( !need_v ) {
		m_vscroll->Hide();
		inner_g->SetTop( m_padding );
	}
	else {
		//Log( std::to_string( m_geometry->m_area.height ) + " / ( " + std::to_string( inner_g->GetEffectiveArea().height ) + " + m_padding * 2 ) // " + std::to_string( inner_g->m_area.top ) + ", " + std::to_string( inner_g->m_area.bottom ) );
		const auto diff = height - outer_h + m_padding * 2;
		m_vscroll->SetScrollType( Scrollbar::ST_VERTICAL );
		m_vscroll->SetMinRaw( 0 );
		if ( m_autoscroll ) {
			needscroll = !m_vscroll->m_is_visible || std::fabs( m_vscroll->m_value - m_vscroll->m_max ) < 0.001f;
		}
		m_vscroll->SetMaxRaw( diff );
		if ( needscroll ) {
			m_vscroll->SetValueRaw( diff );
		}
		m_vscroll->SetSliderSizeByPercentage( (float)outer_h * 0.99f /* TODO: fix properly */ / ( inner_g->GetInnerHeight() ) );
		m_vscroll->Show();
	}

	if ( !need_h ) {
		m_hscroll->Hide();
		inner_g->SetLeft( m_padding );
	}
	else {
		const auto diff = width - outer_w + m_padding * 2;
		m_hscroll->SetScrollType( Scrollbar::ST_HORIZONTAL );
		m_hscroll->SetMinRaw( 0 );
		if ( m_autoscroll ) {
			needscroll = !m_hscroll->m_is_visible || m_hscroll->m_value == m_hscroll->m_max;
		}
		m_hscroll->SetMaxRaw( diff );
		if ( needscroll ) {
			m_hscroll->SetValueRaw( diff );
		}
		m_hscroll->SetSliderSizeByPercentage( (float)m_geometry->m_area.width / ( inner_g->GetInnerWidth() ) );
		m_hscroll->Show();
	}

	if ( m_on_scrollview_resize ) {
		m_on_scrollview_resize();
	}
}

void Scrollview::SetPadding( GSE_CALLABLE, const int padding ) {
	if ( padding != m_padding ) {
		m_padding = padding;
		auto* g = m_inner->GetGeometry();
		UpdateScrollbars( g->m_area.width, g->m_area.height );
	}
}

void Scrollview::SetAutoScroll( GSE_CALLABLE, const bool value ) {
	if ( value != m_autoscroll ) {
		m_autoscroll = value;
	}
}

}
}
