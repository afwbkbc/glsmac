#include "SlidingMenu.h"

#include "game/Settings.h"
#include "engine/Engine.h"

namespace game {
namespace mainmenu {

SlidingMenu::SlidingMenu( MainMenu* mainmenu, const std::string& title, const choice_handlers_t& choices )
	: MenuObject( mainmenu, title )
	, m_choices ( choices )
{
	//
}

void SlidingMenu::Show() {
	ASSERT( !m_menu_block, "duplicate menu show" );
	
	std::vector< std::string > choice_texts = {};
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
	ASSERT( m_menu_block, "hide without show" );
	
	g_engine->GetUI()->RemoveObject( m_menu_block );
	m_menu_block = nullptr;
}

void SlidingMenu::OnItemClick( const std::string& choice ) {
	for (auto& it : m_choices) {
		if (it.first == choice) {
			it.second();
			return;
		}
	}
}

const std::string SlidingMenu::GetChoice() const {
	if ( m_menu_block ) {
		return m_menu_block->GetChoice();
	}
	else {
		return "";
	}
}

void SlidingMenu::SetChoice( const std::string& choice ) {
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

bool SlidingMenu::MaybeClose() {
	Close();
	return true;
}

}
}
