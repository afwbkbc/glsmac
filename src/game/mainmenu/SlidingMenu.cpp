#include "SlidingMenu.h"

#include <stdexcept>

#include "game/Settings.h"
#include "engine/Engine.h"

using namespace std;

namespace game {
namespace mainmenu {

SlidingMenu::SlidingMenu( MainMenu* mainmenu, const string& title, const choice_handlers_t& choices )
	: MenuObject( mainmenu, title )
	, m_choices ( choices )
{
	//
}

void SlidingMenu::Show() {
#if DEBUG
	if ( m_menu_block ) {
		throw runtime_error( "duplicate menu show" );
	}
#endif
	vector<string> choice_texts = {};
	for (auto& it : m_choices) {
		choice_texts.push_back( it.first );
	}

	NEW( m_menu_block, MenuBlock, this );
	for (auto& c : choice_texts) {
		m_menu_block->AddItem(c);
	}
	if (!m_title.empty()) {
		m_menu_block->AddTitle( m_title );
	}
	g_engine->GetUI()->AddObject( m_menu_block );
}

void SlidingMenu::Hide() {
#if DEBUG
	if ( !m_menu_block ) {
		throw runtime_error( "hide without show" );
	}
#endif
	g_engine->GetUI()->RemoveObject( m_menu_block );
	m_menu_block = nullptr;
}

void SlidingMenu::OnItemClick( const string& choice ) {
	for (auto& it : m_choices) {
		if (it.first == choice) {
			it.second();
			return;
		}
	}
}

const string SlidingMenu::GetChoice() const {
	if ( m_menu_block ) {
		return m_menu_block->GetChoice();
	}
	else {
		return "";
	}
}

void SlidingMenu::SetChoice( const string& choice ) {
	m_choice = choice;
	if ( m_menu_block ) {
		m_menu_block->SetChoice( choice );
	}
}

void SlidingMenu::Close() {
	if ( !m_menu_block->IsSliding() ) {
		m_menu_block->GoBack();
	}
}

}
}
