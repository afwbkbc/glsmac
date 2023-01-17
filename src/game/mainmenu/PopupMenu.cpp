#include "PopupMenu.h"

#include "engine/Engine.h"

using namespace std;

using namespace ui;
namespace ui {
	using namespace object;
}

namespace game {
namespace mainmenu {

PopupMenu::PopupMenu( MainMenu *mainmenu, const string& title )
	: MenuObject( mainmenu, title )
{
}

void PopupMenu::Show() {
	
	NEW( m_frame, Panel, "PopupMenuFrame" );
	g_engine->GetUI()->AddObject( m_frame );
	
	NEW( m_button_ok, Button, "PopupMenuButton" );
	m_button_ok->SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_LEFT );
	m_button_ok->SetLeft( 12 );
	m_button_ok->SetBottom( 15 );
	m_frame->AddChild( m_button_ok );
	
	NEW( m_button_cancel, Button, "PopupMenuButton" );
	m_button_cancel->SetAlign( UIObject::ALIGN_BOTTOM | UIObject::ALIGN_RIGHT );
	m_button_cancel->SetRight( 12 );
	m_button_cancel->SetBottom( 15 );
	m_frame->AddChild( m_button_cancel );
}

void PopupMenu::Hide() {
	m_frame->RemoveChild( m_button_ok );
	m_frame->RemoveChild( m_button_cancel );
	
	g_engine->GetUI()->RemoveObject( m_frame );
}

}
}
