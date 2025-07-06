#include "Scrollbar.h"

#include "Button.h"

#include "input/Event.h"
#include "ui/UI.h"
#include "ui/geometry/Geometry.h"

#include "gse/value/Float.h"

namespace ui {
namespace dom {

Scrollbar::Scrollbar( DOM_ARGS_T )
	: Panel( DOM_ARGS_PASS_T, false ) {

	m_from = new Button( GSE_CALL, ui, this, {} );
	ForwardProperty( GSE_CALL, "class_from", "class", m_from );
	Embed( m_from );

	m_to = new Button( GSE_CALL, ui, this, {} );
	ForwardProperty( GSE_CALL, "class_to", "class", m_to );
	Embed( m_to );

	m_slider = new Button( GSE_CALL, ui, this, {} );
	ForwardProperty( GSE_CALL, "class_slider", "class", m_slider );
	Embed( m_slider );

	Property(
		GSE_CALL, "scroll_type", gse::Value::T_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto value = ( (gse::value::String*)v )->value;
			scroll_type_t st;
			if ( value == "vertical" ) {
				st = ST_VERTICAL;
			}
			else if ( value == "horizontal" ) {
				st = ST_HORIZONTAL;
			}
			else {
				GSE_ERROR( gse::EC.TYPE_ERROR, "Unknown scroll_type: " + value + ". Available: vertical horizontal" );
			}
			if ( st != m_scroll_type ) {
				m_scroll_type = st;
				Resize();
				RealignSlider();
			}
		},
		[ this ]( GSE_CALLABLE ) {
			if ( m_scroll_type != ST_VERTICAL ) {
				m_scroll_type = ST_VERTICAL;
				Resize();
				RealignSlider();
			}
		}
	);

	PROPERTY( "min", gse::value::Float, 0.0f, SetMin );
	PROPERTY( "max", gse::value::Float, 100.0f, SetMax );
	PROPERTY( "value", gse::value::Float, 50.0f, SetValue );
	PROPERTY( "speed", gse::value::Float, 1.0f, SetSpeed );

	Property(
		GSE_CALL, "fromto_size", gse::Value::T_INT, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto value = ( (gse::value::Int*)v )->value;
			if ( value != m_fromto_size ) {
				m_fromto_size = value;
				ResizeFromTo();
				RealignSlider();
			}
		},
		[ this ]( GSE_CALLABLE ) {
			if ( m_fromto_size != 0 ) {
				m_fromto_size = 0;
				ResizeFromTo();
				RealignSlider();
			}
		}
	);

	Property(
		GSE_CALL, "slider_size", gse::Value::T_INT, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			const auto value = ( (gse::value::Int*)v )->value;
			if ( value != m_slider_size ) {
				m_slider_size = value;
				ResizeSlider();
				RealignSlider();
			}
		},
		[ this ]( GSE_CALLABLE ) {
			if ( m_slider_size != 0 ) {
				m_slider_size = 0;
				ResizeSlider();
				RealignSlider();
			}
		}
	);

	GeometryHandler(
		GH_ON_AREA_UPDATE, [ this ]() {
			Resize();
			RealignSlider();
		}
	);

	m_from->m_on_mousedown = [ this ]( const input::Event& event ) {
		m_arrow_scroll.direction = -1.0f;
		m_arrow_scroll.timer.SetInterval( m_arrow_scroll.frequency_ms );
		Scroll();
		return true;
	};
	m_to->m_on_mousedown = [ this ]( const input::Event& event ) {
		m_arrow_scroll.direction = 1.0f;
		m_arrow_scroll.timer.SetInterval( m_arrow_scroll.frequency_ms );
		Scroll();
		return true;
	};
	m_from->m_on_mouseup = m_to->m_on_mouseup = [ this ]( const input::Event& event ) {
		m_arrow_scroll.timer.Stop();
		return true;
	};

	Iterable(
		[ this ]() {
			while ( m_arrow_scroll.timer.HasTicked() ) {
				Scroll();
			}
		}
	);

	m_slider->m_on_mousedown = [ this ]( const input::Event& event ) {
		switch ( m_scroll_type ) {
			case ST_VERTICAL: {
				const auto& area = m_slider->GetGeometry()->m_area;
				m_slider_drag.initial_offset = event.data.mouse.y - area.top;
				break;
			}
			case ST_HORIZONTAL: {
				THROW( "TODO: HORIZONTAL" );
				break;
			}
			default:
				ASSERT( false, "Unknown scrollbar type: " + std::to_string( m_scroll_type ) );
		}
		m_slider_drag.is_dragging = true;
		return true;
	};
	m_slider->m_on_mouseup = [ this ]( const input::Event& event ) {
		m_slider_drag.is_dragging = false;
		return false;
	};
}

const bool Scrollbar::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	if ( m_slider_drag.is_dragging ) {
		if ( event.type == input::EV_MOUSE_MOVE ) {
			const auto offset = event.data.mouse.y - m_geometry->m_area.top - m_slider_drag.initial_offset;
			auto mintop = m_fromto_size;
			auto maxtop = m_geometry->m_area.height - m_fromto_size * 2;
			SetValue( GSE_CALL, ( offset - mintop ) * ( m_max - m_min ) / ( maxtop - m_slider_size ) + m_min, true );
		}
		else if ( event.type == input::EV_MOUSE_UP ) {
			m_slider_drag.is_dragging = false;
		}
	}
	return Panel::ProcessEvent( GSE_CALL, event );
}

void Scrollbar::SetMin( GSE_CALLABLE, const float min ) {
	if ( min != m_min ) {
		if ( min >= m_max ) {
			GSE_ERROR( gse::EC.TYPE_ERROR, "Scrollbar min value ( " + std::to_string( min ) + " ) must be smaller than it's max value ( " + std::to_string( m_max ) + " )" );
		}
		m_min = min;
		if ( m_value < m_min ) {
			SetValue( GSE_CALL, m_min, true );
		}
		else {
			RealignSlider();
		}
	}
}

void Scrollbar::SetMax( GSE_CALLABLE, const float max ) {
	if ( max != m_max ) {
		if ( max <= m_min ) {
			GSE_ERROR( gse::EC.TYPE_ERROR, "Scrollbar max value ( " + std::to_string( max ) + " ) must be larger than it's min value ( " + std::to_string( m_min ) + " )" );
		}
		m_max = max;
		if ( m_value > m_max ) {
			SetValue( GSE_CALL, m_max, true );
		}
		else {
			RealignSlider();
		}
	}
}

void Scrollbar::SetValue( GSE_CALLABLE, float value, const bool send_event ) {
	if ( value < m_min ) {
		value = m_min;
	}
	else if ( value > m_max ) {
		value = m_max;
	}
	if ( value != m_value ) {
		m_value = value;
		RealignSlider();
		if ( send_event ) {
			gse::value::object_properties_t event_data = {
				{ "value", VALUE( gse::value::Float, , m_value ) },
			};
			Trigger( GSE_CALL, "change", ARGS( event_data ) );
		}
	}
}

void Scrollbar::SetSpeed( GSE_CALLABLE, float speed ) {
	if ( speed != m_speed ) {
		m_speed = speed;
	}
}

void Scrollbar::Resize() {
	ResizeFromTo();
	ResizeSlider();
}

void Scrollbar::ResizeFromTo() {
	switch ( m_scroll_type ) {
		case ST_VERTICAL: {

			auto* g_from = m_from->GetGeometry();
			g_from->SetAlign( geometry::Geometry::ALIGN_TOP_CENTER );
			g_from->SetLeft( 0 );
			g_from->SetRight( 0 );
			g_from->SetTop( 0 );
			g_from->SetHeight( m_fromto_size );

			auto* g_to = m_to->GetGeometry();
			g_to->SetAlign( geometry::Geometry::ALIGN_BOTTOM_CENTER );
			g_to->SetLeft( 0 );
			g_to->SetRight( 0 );
			g_to->SetBottom( 0 );
			g_to->SetHeight( m_fromto_size );

			break;
		}
		case ST_HORIZONTAL: {
			THROW( "TODO: HORIZONTAL" );
			break;
		}
		default:
			ASSERT( false, "Unknown scrollbar type: " + std::to_string( m_scroll_type ) );
	}
}

void Scrollbar::ResizeSlider() {
	switch ( m_scroll_type ) {
		case ST_VERTICAL: {
			auto* g = m_slider->GetGeometry();
			g->SetAlign( geometry::Geometry::ALIGN_TOP_CENTER );
			g->SetLeft( 0 );
			g->SetRight( 0 );
			g->SetHeight( m_slider_size );
			break;
		}
		case ST_HORIZONTAL: {
			THROW( "TODO: HORIZONTAL" );
			break;
		}
		default:
			ASSERT( false, "Unknown scrollbar type: " + std::to_string( m_scroll_type ) );
	}
}

void Scrollbar::RealignSlider() {
	switch ( m_scroll_type ) {
		case ST_VERTICAL: {
			size_t mintop = m_fromto_size;
			size_t maxtop = m_geometry->m_area.height - m_fromto_size * 2;

			m_slider->GetGeometry()->SetTop(
				mintop +
					(
						maxtop -
							m_slider_size
					) * ( m_value - m_min ) / ( m_max - m_min )
			);

			break;
		}
		case ST_HORIZONTAL: {
			THROW( "TODO: HORIZONTAL" );
			break;
		}
		default:
			ASSERT( false, "Unknown scrollbar type: " + std::to_string( m_scroll_type ) );
	}
}

void Scrollbar::Scroll() {
	m_ui->WithGSE(
		[ this ]( GSE_CALLABLE ) {
			SetValue( GSE_CALL, m_value + m_arrow_scroll.direction * m_speed * (float)m_arrow_scroll.frequency_ms / 1000.0f, true );
		}
	);
}

}
}
