#include "MenuObject.h"

namespace game {
namespace mainmenu {

MenuObject::MenuObject( MainMenu *mainmenu, const string& title )
	: base::Base()
	, m_mainmenu( mainmenu )
	, m_title( title )
{
	//
}

void MenuObject::NextMenu( MenuObject* menu_object ) {
	m_mainmenu->ShowMenu( menu_object );
}

void MenuObject::GoBack() {
	m_mainmenu->GoBack();
}

void MenuObject::MenuError( const string& error_text ) {
	m_mainmenu->MenuError( error_text );
}

void MenuObject::Close() {
	GoBack();
}

}
}
