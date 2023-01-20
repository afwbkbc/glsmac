#include "MenuObject.h"

using namespace std;

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

void MenuObject::Close() {
	GoBack();
}

}
}
