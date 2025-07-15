#include "Scrollview.h"

#include "Scrollbar.h"

#include "input/Event.h"
#include "ui/UI.h"
#include "ui/geometry/Geometry.h"

#include "gse/value/Float.h"

namespace ui {
namespace dom {

Scrollview::Scrollview( DOM_ARGS_T )
	: Panel( DOM_ARGS_PASS_T, true ) {

	Events(
		{
			//input::EV_MOUSE_SCROLL // TODO
		}
	);

	m_inner = new Container( GSE_CALL, ui, this, {}, "inner", false );
	auto* g = m_inner->GetGeometry();
	g->SetWidth( 0 );
	g->SetHeight( 0 );
	g->SetOverflowMode( geometry::Geometry::OM_RESIZE );
	g->m_on_resize = [ this ]( const size_t width, const size_t height ) {
		Log( "RESIZED " + std::to_string( width ) + "x" + std::to_string( height ) );
	};
	Embed( m_inner );
	m_factory_owner = m_inner;

	m_geometry->SetOverflowMode( geometry::Geometry::OM_HIDDEN );

}

const bool Scrollview::ProcessEvent( GSE_CALLABLE, const input::Event& event ) {
	/*if ( event.type == input::EV_MOUSE_SCROLL && !m_arrow_scroll.timer.IsRunning() ) {
		float value = m_value - m_speed * event.data.mouse.scroll_y * m_wheel_scroll.speed;
		if ( value < m_min ) {
			value = m_min;
		}
		else if ( value > m_max ) {
			value = m_max;
		}
		if ( value != m_value ) {
			m_wheel_scroll.scroller.Scroll( m_value, value, m_wheel_scroll.duration_ms );
		}
		return true;
	}*/
	return Panel::ProcessEvent( GSE_CALL, event );
}

}
}
