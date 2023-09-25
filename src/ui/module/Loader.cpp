#include "Loader.h"

#define MAX_DOTS 3
#define DOTS_CHANGE_INTERVAL 200

namespace ui {
namespace module {

Loader::Loader( UI* ui )
	: Module( ui ) {
	//
}

Loader::~Loader() {
	Hide();
}

void Loader::Show( const std::string& text, const loader_cancel_handler_t on_cancel ) {
	ASSERT( !m_is_active, "loader already running" );
	SetText( text );
	m_on_cancel = on_cancel;

	Start();
}

void Loader::Hide() {
	if ( m_is_active ) {
		Stop();
	}
}

void Loader::Start() {

	if ( m_is_active ) {
		return; // already running
	}

	// TODO: check if started from main thread

	m_is_cancelable = m_on_cancel != 0;

	m_ui->BlockEvents(); // don't allow anything else while loader is visible

	NEW( m_section, Section, "DefaultPopupFrame" );
	m_section->SetTitleText( "PLEASE WAIT" );
	m_section->SetAlign( UIObject::ALIGN_CENTER );
	m_section->SetWidth( 560 );
	m_section->SetHeight( 150 );
	m_section->SetZIndex( 0.9 );
	m_ui->AddObject( m_section );

	NEW( m_label, Label, "DefaultPopupLabel" );
	m_label->SetAlign( UIObject::ALIGN_HCENTER | UIObject::ALIGN_TOP );
	m_label->SetTop( 36 );
	m_label->SetText( m_loading_text );
	m_label->ForwardStyleAttributesV(
		{
			Style::A_FONT,
			Style::A_TEXT_COLOR
		}
	);
	m_section->AddChild( m_label );

	if ( m_is_cancelable ) {
		NEW( m_button_cancel, Button, "DefaultPopupButton" );
		m_button_cancel->SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_HCENTER );
		m_button_cancel->SetBottom( 15 );
		m_button_cancel->SetLabel( "CANCEL" );
		m_button_cancel->On(
			UIEvent::EV_BUTTON_CLICK, EH( this ) {
				Cancel();
				return true;
			}
		);
		m_section->AddChild( m_button_cancel );
		m_section->On(
			UIEvent::EV_KEY_DOWN, EH( this ) {
				if (
					!data->key.modifiers &&
						data->key.code == UIEvent::K_ESCAPE
					) {
					if ( m_is_cancelable ) {
						Cancel();
					}
					return true;
				}
				return true;
			}
		);
	}

	m_dots_timer.SetInterval( DOTS_CHANGE_INTERVAL );

	m_ui->AddIterativeObject(
		this, UH( this ) {
			if ( m_loading_text_change.is_changed ) {
				std::lock_guard< std::mutex > guard( m_loading_text_change.mutex );
				m_loading_text = m_loading_text_change.value;
				m_loading_text_change.is_changed = false;
			}
			if ( m_label ) {
				m_label->SetText( m_loading_text + GetDots() );
			}
		}
	);

	Activate();
}

void Loader::Stop() {
	ASSERT( m_is_active, "loader not running" );

	m_ui->UnblockEvents();

	m_ui->RemoveIterativeObject( this );
	m_dots_timer.Stop();

	m_section->RemoveChild( m_label );
	m_label = nullptr;

	if ( m_button_cancel ) {
		m_section->RemoveChild( m_button_cancel );
		m_button_cancel = nullptr;
	}

	m_ui->RemoveObject( m_section );
	m_section = nullptr;

	m_on_cancel = 0;

	Deactivate();
}

void Loader::Cancel() {
	if ( !m_on_cancel || m_on_cancel() ) {
		Stop();
	}
	else {
		m_button_cancel->Hide();
	}
}

void Loader::SetText( const std::string& loading_text ) {
	std::lock_guard< std::mutex > guard( m_loading_text_change.mutex );
	if ( loading_text != m_loading_text_change.value ) {
		m_loading_text_change.value = loading_text;
		m_loading_text_change.is_changed = true;
	}
}

void Loader::SetIsCancelable( const bool is_cancelable ) {
	if ( m_is_cancelable != is_cancelable ) {
		m_is_cancelable = is_cancelable;
		if ( m_is_cancelable ) {
			ASSERT( m_button_cancel, "dynamic cancelable not implemented yet" );
			m_button_cancel->Show();
		}
		else {
			m_button_cancel->Hide();
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
	return std::string( m_dots_count, '.' ) + std::string( ( MAX_DOTS - m_dots_count ) * 4, ' ' );
}

void Loader::ProcessEvent( event::UIEvent* event ) {
	ASSERT( m_section, "loader section not set" );
	m_section->ProcessEvent( event );
}

}
}
