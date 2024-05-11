#include "Error.h"

#include "ui/UI.h"
#include "ui/theme/Types.h"

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/Button.h"

#define MAX_DOTS 3
#define DOTS_CHANGE_INTERVAL 200

namespace ui {
namespace module {

Error::Error( UI* ui )
	: Module( ui ) {
	//
}

Error::~Error() {
	if ( m_on_close ) {
		m_on_close();
	}
	Hide();
}

void Error::Show( const std::string& text, const ui_handler_t on_close ) {
	//ASSERT( !m_is_active, "error already visible" );
	SetText( text );
	m_on_close = on_close;

	SetText( text );

	Start();
}

void Error::Hide() {
	if ( m_is_active ) {
		Stop();
	}
}

void Error::Start() {

	if ( m_is_active ) {
		return; // already running
	}

	m_ui->BlockEvents(); // don't allow anything else while error is visible

	NEW( m_section, object::Section, "DefaultPopupFrame" );
	m_section->SetTitleText( "ERROR" );
	m_section->SetAlign( ALIGN_CENTER );
	m_section->SetWidth( 560 );
	m_section->SetHeight( 150 );
	m_section->SetZIndex( 0.9 );
	m_ui->AddObject( m_section );

	NEW( m_label, object::Label, "DefaultPopupLabel" );
	m_label->SetAlign( ALIGN_HCENTER | ALIGN_TOP );
	m_label->SetTop( 36 );
	m_label->SetText( m_error_text );
	m_label->ForwardStyleAttributesV(
		{
			A_FONT,
			A_TEXT_COLOR
		}
	);
	m_section->AddChild( m_label );

	NEW( m_button_close, object::Button, "DefaultPopupButton" );
	m_button_close->SetAlign( ALIGN_BOTTOM | ALIGN_HCENTER );
	m_button_close->SetBottom( 15 );
	m_button_close->SetLabel( "OK" );
	m_button_close->On(
		ui::event::EV_BUTTON_CLICK, EH( this ) {
			Close();
			return true;
		}
	);
	m_section->AddChild( m_button_close );

	m_section->On(
		ui::event::EV_KEY_DOWN, EH( this ) {
			if (
				!data->key.modifiers &&
					(
						data->key.code == ui::event::K_ESCAPE |
							data->key.code == ui::event::K_ENTER
					)
				) {
				Close();
				return true;
			}
			return true;
		}
	);

	Activate();
}

void Error::Stop() {
	ASSERT( m_is_active, "error not visible" );

	m_ui->UnblockEvents();

	m_section->RemoveChild( m_label );
	m_label = nullptr;

	m_section->RemoveChild( m_button_close );
	m_button_close = nullptr;

	m_ui->RemoveObject( m_section );
	m_section = nullptr;

	m_on_close = 0;

	Deactivate();
}

void Error::Close() {
	const auto on_close = m_on_close;
	Stop();
	if ( on_close ) {
		on_close();
	}
}

void Error::SetText( const std::string& error_text ) {
	if ( error_text != m_error_text ) {
		m_error_text = error_text;
		if ( m_label ) {
			m_label->SetText( error_text );
		}
	}
}

void Error::ProcessEvent( event::UIEvent* event ) {
	ASSERT( m_section, "error section not set" );
	m_section->ProcessEvent( event );
}

}
}
