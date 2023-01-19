#include "PopupMenu.h"

#include "engine/Engine.h"

using namespace std;

using namespace ui;
namespace ui {
	using namespace object;
	using namespace event;
}

namespace game {
namespace mainmenu {

PopupMenu::PopupMenu( MainMenu *mainmenu, const string& title )
	: MenuObject( mainmenu, title )
{
	
}

void PopupMenu::Show() {
	
	NEW( m_frame, Panel, "PopupMenuFrame" );
		m_frame->SetAlign( UIObject::ALIGN_CENTER );
		m_frame->SetWidth( 500 );
		m_frame->SetHeight( 154 );
	g_engine->GetUI()->AddObject( m_frame );
	
	NEW( m_titleframe, Panel, "PopupMenuFrame" );
		m_titleframe->SetAlign( UIObject::ALIGN_TOP );
		m_titleframe->SetPadding( 4 );
		m_titleframe->SetHeight( 24 );
	m_frame->AddChild( m_titleframe );
	NEW( m_titlebar, Panel, "PopupMenuHeader" );
		m_titlebar->SetAlign( UIObject::ALIGN_CENTER );
		m_titlebar->SetPadding( 3 );
	m_titleframe->AddChild( m_titlebar );
	NEW( m_titlelabel, Label, "PopupMenuHeaderLabel" );
		m_titlelabel->SetText( m_title );
	m_titlebar->AddChild( m_titlelabel );
	
	NEW( m_body, Panel, "PopupMenuFrame" );
		m_body->SetAlign( UIObject::ALIGN_TOP );
		m_body->SetLeft( 4 );
		m_body->SetRight( 4 );
		m_body->SetTop( 30 );
		m_body->SetBottom( 40 );
	m_frame->AddChild( m_body );
	
	NEW( m_button_ok, Button, "PopupMenuButtonOkCancel" );
		m_button_ok->SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
		m_button_ok->SetLeft( 12 );
		m_button_ok->SetBottom( 15 );
		m_button_ok->SetWidth( 234 );
		m_button_ok->SetLabel( "OK" );
		m_button_ok->OnClick( EH( this ) {
			OnOkClick();
		});
	m_frame->AddChild( m_button_ok );
	
	NEW( m_button_cancel, Button, "PopupMenuButtonOkCancel" );
		m_button_cancel->SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
		m_button_cancel->SetRight( 12 );
		m_button_cancel->SetBottom( 15 );
		m_button_cancel->SetWidth( 234 );
		m_button_cancel->SetLabel( "CANCEL" );
		m_button_cancel->OnClick( EH( this ) {
			GoBack();
		});
	m_frame->AddChild( m_button_cancel );
}

void PopupMenu::Hide() {
			m_titlebar->RemoveChild( m_titlelabel );
		m_titleframe->RemoveChild( m_titlebar );
	m_frame->RemoveChild( m_titleframe );
	
	m_frame->RemoveChild( m_body );
	
	m_frame->RemoveChild( m_button_ok );
	m_frame->RemoveChild( m_button_cancel );
	
	g_engine->GetUI()->RemoveObject( m_frame );
}

}
}
