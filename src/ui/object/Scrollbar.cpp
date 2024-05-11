#include "Scrollbar.h"

#include "engine/Engine.h"
#include "ui/UI.h"
#include "Surface.h"
#include "SimpleButton.h"

namespace ui {
namespace object {

void Scrollbar::Create() {
	UIContainer::Create();

	NEW( m_background, ui::object::Surface );
	m_background->ForwardStyleAttribute( A_TEXTURE_BACK, A_TEXTURE );
	AddChild( m_background );

	const attribute_type_t sizeattr = m_direction == SD_VERTICAL
		? A_WIDTH
		: A_HEIGHT;

	ASSERT( m_direction == SD_VERTICAL, "only vertical scrollbar is implemented for now" );

	NEW( m_up, SimpleButton );
	m_up->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
	m_up->ForwardStyleAttribute( A_TEXTURE_TOP, A_TEXTURE );
	m_up->ForwardStyleAttribute( sizeattr, A_WIDTH );
	m_up->ForwardStyleAttribute( sizeattr, A_HEIGHT );
	m_up->On(
		event::EV_MOUSE_DOWN, EH( this ) {
			m_arrow_scroll.value = 1;
			ProcessArrowScroll();
			m_arrow_scroll.timer.SetInterval( m_arrow_scroll.frequency_ms );
			return false;
		}
	);
	m_up->On(
		{
			event::EV_MOUSE_UP,
			event::EV_MOUSE_OUT
		}, EH( this ) {
			m_arrow_scroll.timer.Stop();
			return false;
		}
	);
	AddChild( m_up );

	NEW( m_slider, SimpleButton );
	m_slider->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
	m_slider->ForwardStyleAttribute( A_TEXTURE_CENTER, A_TEXTURE );
	m_slider->ForwardStyleAttribute( sizeattr, A_WIDTH );
	m_slider->ForwardStyleAttribute( sizeattr, A_HEIGHT );

	// TODO: global mouse drag event
	m_slider->On(
		event::EV_MOUSE_DOWN, EH( this ) {
			m_slider_scroll.is_dragging = true;
			m_slider_scroll.drag_rel_position = {
				data->mouse.absolute.x - (ssize_t)m_slider->GetLeft(),
				data->mouse.absolute.y - (ssize_t)m_slider->GetTop()
			};
			m_slider->AddStyleModifier( M_ACTIVE );
			return true;
		}
	);
	auto* ui = g_engine->GetUI();
	m_slider_scroll.on_mouse_up = ui->AddGlobalEventHandler(
		event::EV_MOUSE_UP, EH( this ) {
			if ( m_slider_scroll.is_dragging ) {
				m_slider_scroll.is_dragging = false;
				m_slider->RemoveStyleModifier( M_ACTIVE );
				event::event_data_t d = {};
				d.value.scroll.is_scrolling = false;
				Trigger( event::EV_CHANGE, &d );
			}
			return false;
		}, UI::GH_BEFORE
	);
	m_slider_scroll.on_mouse_move = ui->AddGlobalEventHandler(
		event::EV_MOUSE_MOVE, EH( this ) {
			if ( m_slider_scroll.is_dragging ) {
				const types::Vec2< ssize_t > drag_position = {
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
				return true;
			}
			return false;
		}, UI::GH_BEFORE
	);

	AddChild( m_slider );

	NEW( m_down, SimpleButton );
	m_down->SetAlign( ALIGN_HCENTER | ALIGN_BOTTOM );
	m_down->ForwardStyleAttribute( A_TEXTURE_BOTTOM, A_TEXTURE );
	m_down->ForwardStyleAttribute( sizeattr, A_WIDTH );
	m_down->ForwardStyleAttribute( sizeattr, A_HEIGHT );
	m_down->On(
		event::EV_MOUSE_DOWN, EH( this ) {
			m_arrow_scroll.value = -1;
			ProcessArrowScroll();
			m_arrow_scroll.timer.SetInterval( m_arrow_scroll.frequency_ms );
			return false;
		}
	);
	m_down->On(
		{
			event::EV_MOUSE_UP,
			event::EV_MOUSE_OUT
		}, EH( this ) {
			m_arrow_scroll.timer.Stop();
			return false;
		}
	);
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

const float Scrollbar::GetPercentage() const {
	return m_percentage;
}

void Scrollbar::ProcessArrowScroll() {
	// emulate mouse scroll
	event::event_data_t d = {};
	d.mouse.scroll_y = m_arrow_scroll.value;
	// TODO: mouse coordinates?
	Trigger( event::EV_MOUSE_SCROLL, &d );
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
				100.0f;

	if ( percentage != m_percentage ) {
		m_percentage = percentage;
		event::event_data_t d = {};
		d.value.scroll.is_scrolling = true;
		d.value.scroll.percentage = m_percentage;
		Trigger( event::EV_CHANGE, &d );
	}
}

void Scrollbar::RealignSlider() {
	ASSERT( m_slider, "slider not created" );
	ASSERT( m_direction == SD_VERTICAL, "only vertical scrollbar is implemented for now" );

	const auto& area = GetInternalObjectArea();
	const auto& up_area = m_up->GetObjectArea();
	const auto& slider_area = m_slider->GetObjectArea();
	const auto& down_area = m_down->GetObjectArea();

	//Log( "Setting slider percentage " + std::to_string( m_percentage ) );

	m_slider->SetTop(
		+up_area.height
			+ m_up->GetTop()
			+ (
				area.height
					- up_area.height
					- m_up->GetTop()
					- down_area.height
					- m_down->GetBottom()
					- slider_area.height
			) * m_percentage / 100.0f
	);
	//Log( "Slider top changed to " + std::to_string( m_slider->GetTop() ) );
}

}
}
