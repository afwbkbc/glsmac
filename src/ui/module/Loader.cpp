#include "Loader.h"

#include "engine/Engine.h"

#define MAX_DOTS 3
#define DOTS_CHANGE_INTERVAL 200

namespace ui {
namespace module {

void Loader::Start() {
	
	if ( m_is_iterating ) {
		return; // already running
	}
	
	m_is_iterating = true;
	
	auto* ui = g_engine->GetUI();
	
	ui->BlockEvents(); // don't allow anything else while loader is visible
	
	ASSERT( m_handlers.on_start, "on_start not set" );
	ASSERT( m_handlers.on_iterate, "on_iterate not set" );
	ASSERT( m_handlers.on_stop, "on_stop not set" );
	
	NEW( m_section, Section, "Loader" );
		m_section->SetTitleText( "PLEASE WAIT" );
		m_section->SetAlign( UIObject::ALIGN_CENTER );
		m_section->SetWidth( 500 );
		m_section->SetHeight( 150 );
		m_section->SetZIndex( 0.9 );
	ui->AddObject( m_section );
	
	NEW( m_label, Label, "LoaderLabel" );
		m_label->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
		m_label->SetTop( 36 );
		m_label->SetLeft( 80 );
		m_label->SetText( m_loading_text );
		m_label->ForwardStyleAttributesV({ Style::A_FONT, Style::A_TEXTCOLOR });
	m_section->AddChild( m_label );
	
	NEW( m_button_cancel, Button, "LoaderButton" );
			m_button_cancel->SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_HCENTER );
			m_button_cancel->SetBottom( 15 );
			m_button_cancel->SetLabel( "CANCEL" );
			m_button_cancel->On( UIEvent::EV_BUTTON_CLICK, EH( this ) {
				while ( !m_handlers.on_stop( this ) ) {
					std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
				}
				Stop();
				return true;
			});
	m_section->AddChild( m_button_cancel );
	
	m_dots_timer.SetInterval( DOTS_CHANGE_INTERVAL );
	
	ui->AddIterativeObject( this, IH( this ) {
		if ( !m_handlers.on_iterate( this ) ) {
			Stop();
		}
	});
	
	if ( !m_handlers.on_start( this ) ) {
		Stop();
	}

}

void Loader::Stop() {
	if ( !m_is_iterating ) {
		return; // already stopped
	}
	
	m_is_iterating = false;
	
	auto* ui = g_engine->GetUI();
	
	ui->UnblockEvents();
	
	ui->RemoveIterativeObject( this );
	m_dots_timer.Stop();
	
	m_handlers.on_start = nullptr;
	m_handlers.on_iterate = nullptr;
	m_handlers.on_stop = nullptr;
	
		m_section->RemoveChild( m_label );
		m_section->RemoveChild( m_button_cancel );
	ui->RemoveObject( m_section );

}

const bool Loader::IsRunning() const {
	return m_is_iterating;
}

void Loader::SetOnStart( const loading_handler_t handler ) {
	ASSERT( !m_handlers.on_start, "on_start already set" );
	ASSERT( !m_is_iterating, "can't set handlers on running loader" );
	m_handlers.on_start = handler;
}

void Loader::SetOnIterate( const loading_handler_t handler ) {
	ASSERT( !m_handlers.on_iterate, "on_iterate already set" );
	ASSERT( !m_is_iterating, "can't set handlers on running loader" );
	m_handlers.on_iterate = handler;
}

void Loader::SetOnStop( const loading_handler_t handler ) {
	ASSERT( !m_handlers.on_stop, "on_cancel already set" );
	ASSERT( !m_is_iterating, "can't set handlers on running loader" );
	m_handlers.on_stop = handler;
}

void Loader::SetLoadingText( const std::string& loading_text ) {
	if ( loading_text != m_loading_text ) {
		m_loading_text = loading_text;
		if ( m_label ) {
			m_label->SetText( loading_text );
		}
	}
}

const std::string Loader::GetDots() {
	while ( m_dots_timer.HasTicked() ) {
		m_dots_count++;
		if ( m_dots_count > MAX_DOTS ) {
			m_dots_count = 0;
		}
	}
	return std::string( m_dots_count, '.' ) + std::string( ( MAX_DOTS - m_dots_count ) * 2, ' ' );
}

}
}
