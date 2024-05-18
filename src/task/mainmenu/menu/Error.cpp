#include "Error.h"

#include "ui/object/Label.h"
#include "ui/object/Panel.h"

namespace task {
namespace mainmenu {

Error::Error( MainMenu* mainmenu, const std::string& error_text )
	: PopupMenu( mainmenu, "ERROR" )
	, m_error_text( error_text ) {
	SetWidth( 300 );
	SetHeight( 110 );
	SetFlags( { PF_HAS_OK } );

}

void Error::Show() {
	PopupMenu::Show();

	NEW( m_error_label, ui::object::Label, "PopupText" );
	m_error_label->SetText( m_error_text );
	m_body->AddChild( m_error_label );
}

void Error::Hide() {
	m_body->RemoveChild( m_error_label );

	PopupMenu::Hide();
}

void Error::OnNext() {

	GoBack();
}

}
}
