#include "SlidingMenu.h"

#include "game/backend/settings/Settings.h"
#include "engine/Engine.h"
#include "ui_legacy/slidingmenu/MenuBlock.h"
#include "ui_legacy/UI.h"

namespace task {
namespace mainmenu {

SlidingMenu::SlidingMenu( MainMenu* mainmenu, const std::string& title, const choices_t& choices, const size_t default_choice )
	: MenuObject( mainmenu, title )
	, m_choices( choices )
	, m_default_choice( default_choice ) {
	//
}

void SlidingMenu::Show() {
	ASSERT( !m_menu_block, "duplicate menu show" );

	NEW( m_menu_block, MenuBlock, this );
	for ( auto& c : m_choices ) {
		m_menu_block->AddItem( c.first, c.second );
	}
	if ( !m_title.empty() ) {
		m_menu_block->AddTitle( m_title );
	}
	if ( m_default_choice ) {
		m_menu_block->SelectItem( m_choices.size() - m_default_choice - 1 );
	}
	g_engine->GetUI()->AddObject( m_menu_block );
}

void SlidingMenu::Hide() {
	ASSERT( m_menu_block, "hide without show" );
	g_engine->GetUI()->RemoveObject( m_menu_block );
	m_menu_block = nullptr;
}

void SlidingMenu::OnItemClick( const std::string& choice ) {
	const auto& c = GetChoiceHandlers( choice );
	if ( c.on_click ) {
		c.on_click();
	}
	else {
		MenuError();
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
	if ( IsReadyToClose() ) {
		m_menu_block->GoBack();
	}
}

bool SlidingMenu::MaybeClose() {
	Close();
	return true;
}

bool SlidingMenu::IsReadyToClose() const {
	return !m_menu_block->IsSliding();
}

const choice_handlers_t& SlidingMenu::GetChoiceHandlers( const std::string& choice ) const {
	for ( auto& it : m_choices ) {
		if ( it.first == choice ) {
			return it.second;
		}
	}
	THROW( "choice '" + choice + "' does not exist" );
	return m_choices.front().second;
}

}
}
