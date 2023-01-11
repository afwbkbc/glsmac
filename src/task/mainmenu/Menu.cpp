#include "Menu.h"

using namespace std;

namespace task {
namespace mainmenu {

Menu::Menu( const MainMenuTask* task )
	: UIContainer()
	, m_task( task )
{
	
}

void Menu::AddItem(const string& text) {
	m_items.push_back(text);
}

void Menu::Create() {
	UIContainer::Create();
	
	SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	SetWidth(354);
	
	for (size_t i = m_items.size() ; i > 0 ; i--) {
		auto *item = new MenuItem( this, m_items[i-1] );
		item->SetBottom(m_menu_items.size() * item->GetHeight());
		m_menu_items.push_back(item);
		AddChild(item);
	}
	
}

void Menu::Destroy() {
	
	for (auto& item : m_menu_items) {
		RemoveChild(item);
	}
	m_menu_items.clear();
	
	UIContainer::Destroy();
}

}
}
