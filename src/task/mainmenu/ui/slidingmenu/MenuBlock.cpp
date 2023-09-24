#include "MenuBlock.h"

#include "ui/event/UIEvent.h"

namespace task {
namespace mainmenu {

MenuBlock::MenuBlock( SlidingMenu* menu )
	: UIContainer()
	, m_menu( menu ) {
	SetClass( "SlidingMenuBlock" );

	SetRight( -MENU_CLOSED_POSITION );

	SetEventContexts( EC_KEYBOARD );
}

void MenuBlock::AddItem( const std::string& text, const choice_handlers_t& handlers ) {
	m_selected_item_index = m_items.size();
	m_items.push_back(
		{
			text,
			handlers
		}
	);
}

void MenuBlock::AddTitle( const std::string& text ) {
	m_title = text;
}

void MenuBlock::SelectItem( const size_t index ) {
	m_selected_item_index = index;
}

const std::string MenuBlock::GetChoice() const {
	if ( m_selected_item ) {
		return m_selected_item->GetText();
	}
	else {
		return "";
	}
}

void MenuBlock::SetChoice( const std::string& choice ) {
	for ( auto i = 0 ; i < m_items.size() ; i++ ) {
		if ( m_items[ i ].first == choice ) {
			SetActiveItem( m_items.size() - i - 1 );
			return;
		}
	}
	ASSERT( false, "choice '" + choice + "' not found" );
}

bool MenuBlock::IsSliding() {
	return m_slide_timer.IsRunning();
}

void MenuBlock::GoBack() {
	m_clicked_choice = "";
	m_slide_change = -m_slide_change;
	m_slide_timer.SetInterval( 1 );
}

void MenuBlock::Create() {
	UIContainer::Create();

	ASSERT( !m_items.empty(), "items list empty" );

	// upside down because it's easier to position that way
	for ( size_t i = m_items.size() ; i > 0 ; i-- ) {
		const auto& item = m_items[ i - 1 ];
		NEWV( menu_item, MenuItem, this, item.first );
		menu_item->SetBottom( m_menu_items.size() * 70 );
		menu_item->On(
			UIEvent::EV_MOUSE_MOVE, EH( this, item, i ) {
				SetActiveItem( m_items.size() - i );
				return false;
			}
		);
		m_menu_items.push_back( menu_item );
		AddChild( menu_item );
	}
	if ( !m_title.empty() ) {
		NEWV( item, MenuItem, this, m_title, true );
		item->SetBottom( m_menu_items.size() * 70 + 35 );
		m_menu_items.push_back( item );
		AddChild( item );
	}

	On(
		UIEvent::EV_KEY_DOWN, EH( this ) {
			if ( !data->key.modifiers ) {
				if ( data->key.code == UIEvent::K_UP ) {
					if ( m_selected_item_index < m_items.size() - 1 ) {
						SetActiveItem( m_selected_item_index + 1 );
					}
				}
				else if ( data->key.code == UIEvent::K_DOWN ) {
					if ( m_selected_item_index > 0 ) {
						SetActiveItem( m_selected_item_index - 1 );
					}
				}
				else if ( data->key.code == UIEvent::K_ESCAPE ) {
					GoBack();
				}
				else if ( data->key.code == UIEvent::K_ENTER ) {
					OnItemClick( m_selected_item->GetText() );
				}
				else {
					return false; // not handled
				}
				return true; // handled
			}
			else {
				return false; // not handled
			}
		}
	);

	SetActiveItem( m_selected_item_index );

	NEW( m_sound, SoundEffect, "SlidingMenuSound" );
	AddChild( m_sound );

	m_slide_timer.SetInterval( 1 );
}

void MenuBlock::Destroy() {

	m_slide_timer.Stop();

	RemoveChild( m_sound );

	for ( auto& item : m_menu_items ) {
		RemoveChild( item );
	}
	m_menu_items.clear();
	m_selected_item = nullptr;

	UIContainer::Destroy();
}

void MenuBlock::Iterate() {
	UIContainer::Iterate();

	while ( m_slide_timer.HasTicked() ) {
		m_slide_value += m_slide_change;
		SetRight( m_slide_value );
		if ( m_slide_value == 0 ) {
			// menu fully slided out
			m_slide_timer.Stop();
			break;
		}
		else if ( m_slide_change > 0 ) {
			// menu sliding out
		}
		else {
			// menu sliding in
			if ( m_slide_value <= -MENU_CLOSED_POSITION ) {
				// sliding in finished, menu fully closed
				m_slide_timer.Stop();
				if ( !m_clicked_choice.empty() ) {
					// return clicked choice to mainmenu
					m_menu->OnItemClick( m_clicked_choice );
				}
				else {
					// tell mainmenu to go back
					m_menu->GoBack();
				}
			}
		}
	}
}

void MenuBlock::OnItemClick( const std::string& choice ) {
	if ( !IsSliding() ) { // this is first click of this menu
		// save choice
		m_clicked_choice = choice;
		// start slide-in animation
		m_slide_change = -m_slide_change;
		m_slide_timer.SetInterval( 1 );
	}
}

void MenuBlock::SetActiveItem( const size_t index ) {
	if ( index != m_selected_item_index || !m_selected_item ) {
		ASSERT( index < m_menu_items.size(), "invalid item index" );
		auto* item = m_menu_items[ index ];
		if ( item != m_selected_item ) {
			if ( m_selected_item && m_selected_item->HasStyleModifier( Style::M_SELECTED ) ) {
				m_selected_item->RemoveStyleModifier( Style::M_SELECTED );
			}
			if ( !item->HasStyleModifier( Style::M_SELECTED ) ) {
				item->AddStyleModifier( Style::M_SELECTED );
			}
			m_selected_item = item;
			m_selected_item_index = index;
			const auto& i = m_items[ m_items.size() - index - 1 ];
			if ( i.second.on_select ) {
				i.second.on_select();
			}
		}
	}
}

}
}
