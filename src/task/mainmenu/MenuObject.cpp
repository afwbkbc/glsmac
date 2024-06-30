#include "MenuObject.h"

#include "task/mainmenu/MainMenu.h"

namespace task {
namespace mainmenu {

MenuObject::MenuObject( MainMenu* mainmenu, const std::string& title )
	: common::Class()
	, m_mainmenu( mainmenu )
	, m_title( title ) {
	//
}

void MenuObject::NextMenu( MenuObject* menu_object ) {
	m_mainmenu->ShowMenu( menu_object );
}

void MenuObject::GoBack() {
	m_mainmenu->GoBack();
}

void MenuObject::MenuError( const std::string& error_text ) {
	m_mainmenu->MenuError( error_text );
}

void MenuObject::Close() {
	GoBack();
}

}
}
