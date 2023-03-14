#include "Scrollbar.h"

#include "engine/Engine.h"

namespace ui {
namespace object {

void Scrollbar::Create() {
	UIContainer::Create();
	
	NEW( m_background, Surface );
		m_background->ForwardStyleAttribute( Style::A_TEXTURE_BACK, Style::A_TEXTURE );
	AddChild( m_background );
	
	const Style::attribute_type_t sizeattr = m_direction == SD_VERTICAL
		? Style::A_WIDTH
		: Style::A_HEIGHT
	;
	
	ASSERT( m_direction == SD_VERTICAL, "only vertical scrollbar is implemented for now" );
	
	NEW( m_up, SimpleButton );
		m_up->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
		m_up->ForwardStyleAttribute( Style::A_TEXTURE_TOP, Style::A_TEXTURE );
		m_up->ForwardStyleAttribute( sizeattr, Style::A_WIDTH );
		m_up->ForwardStyleAttribute( sizeattr, Style::A_HEIGHT );
		m_up->On( UIEvent::EV_MOUSE_DOWN, EH( this ) {
			m_arrow_scroll.value = 1;
			ProcessArrowScroll();
			m_arrow_scroll.timer.SetInterval( m_arrow_scroll.frequency_ms );
			return false;
		});
		m_up->On( { UIEvent::EV_MOUSE_UP, UIEvent::EV_MOUSE_OUT }, EH( this ) {
			m_arrow_scroll.timer.Stop();
			return false;
		});
	AddChild( m_up );

	NEW( m_slider, SimpleButton );
		m_slider->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
		m_slider->ForwardStyleAttribute( Style::A_TEXTURE_CENTER, Style::A_TEXTURE );
		m_slider->ForwardStyleAttribute( sizeattr, Style::A_WIDTH );
		m_slider->ForwardStyleAttribute( sizeattr, Style::A_HEIGHT );
		
		// TODO: global mouse drag event
		m_slider->On( UIEvent::EV_MOUSE_DOWN, EH( this ) {
			m_slider_scroll.is_dragging = true;
			m_slider_scroll.drag_rel_position = {
				data->mouse.absolute.x - (ssize_t)m_slider->GetLeft(),
				data->mouse.absolute.y - (ssize_t)m_slider->GetTop()
			};
			m_slider->AddStyleModifier( Style::M_ACTIVE );
			return true;
		});
		auto* ui = g_engine->GetUI();
		m_slider_scroll.on_mouse_up = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_UP, EH( this ) {
			if ( m_slider_scroll.is_dragging ) {
				m_slider_scroll.is_dragging = false;
				m_slider->RemoveStyleModifier( Style::M_ACTIVE );
				UIEvent::event_data_t d = {};
				d.change.scroll.is_scrolling = false;
				Trigger( UIEvent::EV_CHANGE, &d );
			}
			return false;
		}, UI::GH_BEFORE );
		m_slider_scroll.on_mouse_move = ui->AddGlobalEventHandler( UIEvent::EV_MOUSE_MOVE, EH( this ) {
			if ( m_slider_scroll.is_dragging ) {
				const Vec2< ssize_t > drag_position = {
					data->mouse.absolute.x,
					data->mouse.absolute.y
				};
				ssize_t newtop = ( drag_position.y - m_slider_scroll.drag_rel_position.y );
				const auto& area = GetInternalObjectArea();
				const auto& area_up = m_up->GetObjectArea();
				const auto& area_slider = m_slider->GetObjectArea();
				const auto& area_down = m_down->GetObjectArea();
				newtop = std::max< ssize_t >( area_up.height, newtop );
				newtop = std::min< ssize_t >( area.height - area_down.height - area_slider.height, newtop );
				if ( newtop != m_slider->GetTop() ) {
					m_slider->SetTop( newtop );
					UpdatePercentage();
				}
			}
			return false;
		}, UI::GH_BEFORE );
		
	AddChild( m_slider );
	
	NEW( m_down, SimpleButton );
		m_down->SetAlign( ALIGN_HCENTER | ALIGN_BOTTOM );
		m_down->ForwardStyleAttribute( Style::A_TEXTURE_BOTTOM, Style::A_TEXTURE );
		m_down->ForwardStyleAttribute( sizeattr, Style::A_WIDTH );
		m_down->ForwardStyleAttribute( sizeattr, Style::A_HEIGHT );
		m_down->On( UIEvent::EV_MOUSE_DOWN, EH( this ) {
			m_arrow_scroll.value = -1;
			ProcessArrowScroll();
			m_arrow_scroll.timer.SetInterval( m_arrow_scroll.frequency_ms );
			return false;
		});
		m_down->On( { UIEvent::EV_MOUSE_UP, UIEvent::EV_MOUSE_OUT }, EH( this ) {
			m_arrow_scroll.timer.Stop();
			return false;
		});
	AddChild( m_down );

}

void Scrollbar::Iterate() {
	UIContainer::Iterate();
	
	while ( m_arrow_scroll.timer.HasTicked() ) {
		ProcessArrowScroll();
	}
}

void Scrollbar::Align() {
	UIContainer::Align();
	
	if ( m_slider ) {
		RealignSlider();
	}
}

void Scrollbar::Destroy() {
	
	g_engine->GetUI()->RemoveGlobalEventHandler( m_slider_scroll.on_mouse_up );
	g_engine->GetUI()->RemoveGlobalEventHandler( m_slider_scroll.on_mouse_move );
	
	RemoveChild( m_background );
	RemoveChild( m_up );
	RemoveChild( m_slider );
	RemoveChild( m_down );
	
	UIContainer::Destroy();
}

void Scrollbar::SetPercentage( const float percentage ) {
	if ( m_percentage != percentage ) {
		ASSERT( percentage >= 0.0f, "percentage below 0" );
		ASSERT( percentage <= 100.0f, "percentage above 100" );
		m_percentage = percentage;
		if ( m_slider ) {
			RealignSlider();
		}
	}
}

void Scrollbar::ProcessArrowScroll() {
	// emulate mouse scroll
	UIEvent::event_data_t d = {};
	d.mouse.scroll_y = m_arrow_scroll.value;
	// TODO: mouse coordinates?
	Trigger( UIEvent::EV_MOUSE_SCROLL, &d );
}

void Scrollbar::UpdatePercentage() {
	const auto& area = GetInternalObjectArea();
	const auto& up_area = m_up->GetObjectArea();
	const auto& slider_area = m_slider->GetObjectArea();
	const auto& down_area = m_down->GetObjectArea();
	
	const float percentage =
		( m_slider->GetTop() - up_area.height )
			/
		( area.height - up_area.height - down_area.height - slider_area.height )
			*
		100.0f
	;
	
	if ( percentage != m_percentage ) {
		m_percentage = percentage;
		UIEvent::event_data_t d = {};
		d.change.scroll.is_scrolling = true;
		d.change.scroll.percentage = m_percentage;
		Trigger( UIEvent::EV_CHANGE, &d );
	}
}

void Scrollbar::RealignSlider() {
	ASSERT( m_slider, "slider not created" );
	ASSERT( m_direction == SD_VERTICAL, "only vertical scrollbar is implemented for now" );
	
	const auto& area = GetInternalObjectArea();
	const auto& up_area = m_up->GetObjectArea();
	const auto& slider_area = m_slider->GetObjectArea();
	const auto& down_area = m_down->GetObjectArea();
	
	m_slider->SetTop(
		+ up_area.height + m_up->GetTop()
		+ slider_area.height / (
	
			// TODO: why substracting up_area.height not needed here?
			// TODO: what's 3 ?
			area.height - down_area.height - m_down->GetBottom() - 3
	
		) * m_percentage
	);
	//Log( "Slider top changed to " + std::to_string( m_slider->GetTop() ) );
}

}
}
