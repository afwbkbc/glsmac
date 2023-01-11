#include "Menu.h"

using namespace std;

namespace task {
namespace mainmenu {

Menu::Menu( MainMenuTask* task )
	: UIContainer()
	, m_task( task )
{
	SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	SetWidth(354);
	SetBottom(16);
	SetRight(-MENU_CLOSED_POSITION);
}

void Menu::AddItem(const string& text) {
	m_items.push_back(text);
}

void Menu::Create() {
	UIContainer::Create();
	
	for (size_t i = m_items.size() ; i > 0 ; i--) {
		auto *item = new MenuItem( this, m_items[i-1] );
		item->SetBottom(m_menu_items.size() * ( item->GetHeight() + 24 ) );
		m_menu_items.push_back(item);
		AddChild(item);
	}
	
	m_slide_timer.SetInterval( 1 );
}

void Menu::Destroy() {
	
	m_slide_timer.Stop();
	
	for (auto& item : m_menu_items) {
		RemoveChild(item);
	}
	m_menu_items.clear();
	
	UIContainer::Destroy();
}

void Menu::Iterate() {
	while (m_slide_timer.Ticked()) {
		m_slide_value += m_slide_change;
		SetRight( m_slide_value );
		if (m_slide_value == 0) {
			// menu fully slided out
			m_slide_timer.Stop();
			break;
		}
		else if (m_slide_change > 0) {
			// menu sliding out
		}
		else {
			// menu sliding in
			if (m_slide_value <= -MENU_CLOSED_POSITION) {
				// sliding in finished, menu fully closed
				// return clicked choice to task
				m_slide_timer.Stop();
				m_task->OnMenuClick( m_clicked_choice );
			}
		}
	}
}

void Menu::OnItemClicked( const string& choice ) {
	if (m_slide_change >= 0) { // this is first click of this menu
		// save choice
		m_clicked_choice = choice;
		// start slide-in animation
		m_slide_change = -m_slide_change;
		m_slide_timer.SetInterval( 1 );
	}
}

}
}
