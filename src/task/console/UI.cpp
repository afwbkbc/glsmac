#include "UI.h"

#include "ui_legacy/object/Surface.h"
#include "ui_legacy/object/Label.h"
#include "ui_legacy/object/Input.h"
#include "ui_legacy/object/TextView.h"
#include "ui_legacy/event/KeyDown.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "ui_legacy/UI.h"

namespace task {
namespace console {

const std::string UI::PROMPT_STR = "> ";

static const std::vector< ::ui_legacy::event::event_type_t > s_events_to_block = {
	::ui_legacy::event::EV_MOUSE_MOVE,
	::ui_legacy::event::EV_MOUSE_OVER,
	::ui_legacy::event::EV_MOUSE_OUT,
	::ui_legacy::event::EV_MOUSE_DOWN,
	::ui_legacy::event::EV_MOUSE_UP,
	::ui_legacy::event::EV_MOUSE_SCROLL,
};

UI::UI()
	: ::ui_legacy::object::UIContainer( "Console" ) {}

void UI::Create() {
	::ui_legacy::object::UIContainer::Create();

	m_toggle_handler = g_engine->GetUI()->AddGlobalEventHandler(
		ui_legacy::event::EV_KEY_DOWN, EH( this ) {
			if ( data->key.code == ui_legacy::event::K_GRAVE && !data->key.modifiers ) {
				Toggle();
				return true;
			}
			if ( m_is_active ) {
				switch ( data->key.code ) {
					case ui_legacy::event::K_ESCAPE: {
						HideConsole();
						break;
					}
					case ui_legacy::event::K_ENTER: {
						const auto& v = m_input->GetValue();
						if ( !v.empty() ) {

							m_history->AddLine( PROMPT_STR + v );
							m_history->ScrollToEnd();

							// TODO: PROCESS

							m_input->Clear();
						}
						break;
					}
					default: {
						NEWV( e, ::ui_legacy::event::KeyDown, data->key.code, data->key.key, data->key.modifiers );
						m_input->ProcessEvent( e );
						DELETE( e );
					}
				}
				return true;
			}
			return false;
		}, ui_legacy::UI::GH_BEFORE
	);

	NEW( m_background, ::ui_legacy::object::Surface, SubClass( "Background" ) );
	AddChild( m_background );

	NEW( m_border, ::ui_legacy::object::Surface, SubClass( "Border" ) );
	AddChild( m_border );

	NEW( m_prompt, ::ui_legacy::object::Label, SubClass( "Prompt" ) );
	m_prompt->SetText( PROMPT_STR );
	AddChild( m_prompt );

	NEW( m_input, ::ui_legacy::object::Input, SubClass( "Input" ) );
	m_input->SetMaxLength( 128 ); // TODO: infinite/automatic length
	AddChild( m_input );

	NEW( m_history, ::ui_legacy::object::TextView, SubClass( "History" ) );
	m_history->SetLinesLimit( HISTORY_LINES_LIMIT );
	AddChild( m_history );

	Hide();
}

void UI::Align() {
	::ui_legacy::object::UIContainer::Align();

	const auto h = g_engine->GetGraphics()->GetViewportHeight() / 2;
	if ( m_last_height != h ) {
		SetHeight( h );
		m_last_height = h;
		if ( m_slide.IsRunning() ) {
			m_slide.Stop();
		}
		SetTop( GetTopTarget() );
	}
}

void UI::Iterate() {
	::ui_legacy::object::UIContainer::Iterate();

	if ( m_slide.IsRunning() ) {
		while ( m_slide.HasTicked() ) {
			SetTop( m_slide.GetPosition() );
		}
	}
	else if ( m_is_visible && !m_is_active ) {
		Hide();
		m_is_visible = m_is_active;
	}

	if ( m_history ) {
		std::lock_guard< std::mutex > guard( m_history_mutex );
		for ( const auto& text : m_history_buffer ) {
			m_history->AddLine( text );
		}
		m_history_buffer.clear();
	}
}

void UI::Destroy() {
	RemoveChild( m_background );
	RemoveChild( m_border );
	RemoveChild( m_prompt );
	RemoveChild( m_input );
	RemoveChild( m_history );

	g_engine->GetUI()->RemoveGlobalEventHandler( m_toggle_handler );

	::ui_legacy::object::UIContainer::Destroy();
}

void UI::Log( const std::string& text ) {
	std::lock_guard< std::mutex > guard( m_history_mutex );
	m_history_buffer.push_back( text );
}

void UI::ShowConsole() {
	if ( !m_is_active ) {

		auto* ui = g_engine->GetUI();
		for ( const auto& s : s_events_to_block ) {
			m_event_block_handlers.push_back(
				ui->AddGlobalEventHandler(
					s, EH( this ) {
						// forward to history to allow scrolling
						::ui_legacy::event::UIEvent e = {
							event_type,
							data
						};
						e.m_flags |= ::ui_legacy::event::EF_MOUSE;
						if ( e.m_type == ::ui_legacy::event::EV_MOUSE_DOWN ) {
							int a = 5;
							a++;
						}
						m_history->ProcessEvent( &e );
						return true;
					}, ui_legacy::UI::GH_BEFORE
				)
			);
		}

		ToggleAndSlide();

		if ( !m_is_visible ) {
			m_is_visible = true;
			Show();
		}

		m_history->ScrollToEnd();
	}
}

void UI::HideConsole() {
	if ( m_is_active ) {

		auto* ui = g_engine->GetUI();
		for ( const auto& h : m_event_block_handlers ) {
			ui->RemoveGlobalEventHandler( h );
		}
		m_event_block_handlers.clear();

		ToggleAndSlide();

	}
}

void UI::Toggle() {

	if ( m_is_active ) {
		HideConsole();
	}
	else {
		ShowConsole();
	}

}

const UI::coord_t UI::GetTopTarget() const {
	if ( m_is_active ) {
		return 0;
	}
	else {
		return -GetHeight();
	}
}

void UI::ToggleAndSlide() {
	const auto from = m_slide.IsRunning()
		? m_slide.GetPosition()
		: GetTopTarget();
	m_is_active = !m_is_active;
	const auto to = GetTopTarget();
	const auto duration = fabs( from - to ) / m_last_height * SLIDE_DURATION_MS;
	SetTop( from );
	m_slide.Scroll( from, to, duration );
}

}
}
