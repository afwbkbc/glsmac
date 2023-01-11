#include "MenuBlock.h"

#include "ui/event/UIEvent.h"

using namespace std;

namespace game {
namespace mainmenu {

MenuBlock::MenuBlock( MainMenu* mainmenu )
	: UIContainer()
	, m_mainmenu( mainmenu )
{
	SetAlign( UIObject::ALIGN_RIGHT | UIObject::ALIGN_BOTTOM );
	SetWidth(354);
	SetBottom(16);
	SetRight(-MENU_CLOSED_POSITION);
}

void MenuBlock::AddItem(const string& text) {
	m_items.push_back(text);
}

void MenuBlock::AddTitle(const string& text) {
	m_title = text;
}

bool MenuBlock::IsSliding() {
	return m_slide_timer.Running();
}

void MenuBlock::Create() {
	UIContainer::Create();
	
	for (size_t i = m_items.size() ; i > 0 ; i--) {
		auto *item = new MenuItem( this, m_items[i-1] );
		item->SetBottom(m_menu_items.size() * ( item->GetHeight() + 24 ) );
		m_menu_items.push_back(item);
		AddChild(item);
	}
	if (!m_title.empty()) {
		auto *item = new MenuItem( this, m_title, true );
		item->SetBottom(m_menu_items.size() * ( item->GetHeight() + 32 ) );
		m_menu_items.push_back(item);
		AddChild(item);
	}
	
	m_slide_timer.SetInterval( 1 );
}

void MenuBlock::Destroy() {
	
	m_slide_timer.Stop();
	
	for (auto& item : m_menu_items) {
		RemoveChild(item);
	}
	m_menu_items.clear();
	
	UIContainer::Destroy();
}

void MenuBlock::Iterate() {
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
				m_slide_timer.Stop();
				if ( !m_clicked_choice.empty() ) {
					// return clicked choice to mainmenu
					m_mainmenu->OnMenuClick( m_clicked_choice );
				}
				else {
					// tell mainmenu to go back
					m_mainmenu->GoBack();
				}
			}
		}
	}
}

void MenuBlock::OnKeyDown( const UIEvent::event_data_t* data ) {
	if (!IsSliding()) {
		if (data->key.code == UIEvent::K_ESCAPE) {
			// means 'go back'
			m_clicked_choice = "";
			m_slide_change = -m_slide_change;
			m_slide_timer.SetInterval( 1 );
		}
	}
}

void MenuBlock::OnItemClick( const string& choice ) {
	if (!IsSliding()) { // this is first click of this menu
		// save choice
		m_clicked_choice = choice;
		// start slide-in animation
		m_slide_change = -m_slide_change;
		m_slide_timer.SetInterval( 1 );
	}
}

}
}
