#include "Menu.h"

#include "engine/Engine.h"
#include "../Game.h"

namespace task {
namespace game {
namespace ui {

Menu::Menu( Game* game, const uint8_t item_height, const uint8_t margin )
	: UI( game, "BBLeftMenu" )
	, m_item_height( item_height )
	, m_margin( margin ) {
	Hide(); // closed by default
}

Menu::~Menu() {
	for ( auto& item : m_menu_items ) {
		if ( item.submenu ) {
			g_engine->GetUI()->RemoveObject( item.submenu );
		}
	}
}

void Menu::Create() {
	UI::Create();

	Log( "Creating side menu" );

	NEW( m_background, ::ui::object::Surface, "BBMenuBackground" );
	AddChild( m_background );

	NEW( m_frames.top, ::ui::object::Surface, "BBMenuTopFrame" );
	AddChild( m_frames.top );

	NEW( m_frames.bottom, ::ui::object::Surface, "BBMenuBottomFrame" );
	AddChild( m_frames.bottom );

	size_t top = m_margin;
	for ( auto& item : m_menu_items ) {
		NEWV( button, ::ui::object::LabelButton, "BBMenuButton" );
		button->SetHeight( m_item_height );
		button->SetTop( top );
		button->SetLabel( item.label );
		button->SetAreaLimitsByObject( this );
		button->On(
			UIEvent::EV_BUTTON_CLICK, EH( this, item, button ) {
				if ( m_active_button && m_active_button != button ) {
					m_active_button->RemoveStyleModifier( Style::M_SELECTED );
					m_active_button = nullptr;
				}
				if ( m_active_submenu && m_active_submenu != item.submenu ) {
					m_active_submenu->Hide();
					m_active_submenu = nullptr;
				}
				if ( item.submenu ) {
					if ( button->HasStyleModifier( Style::M_SELECTED ) ) {
						button->RemoveStyleModifier( Style::M_SELECTED );
						item.submenu->Hide();
						m_active_button = nullptr;
						m_active_submenu = nullptr;
					}
					else {
						m_active_button = button;
						m_active_button->AddStyleModifier( Style::M_SELECTED );
						item.submenu->SetBottom( GetBottom() + GetHeight() - ( button->GetTop() + button->GetHeight() ) - m_margin );
						item.submenu->Show();
						m_active_submenu = item.submenu;
					}
				}
				else {
					if ( item.on_click ) {
						if ( item.on_click( button, item ) ) {
							m_game->CloseMenus();
						}
					}
				}
				return true;
			}
		);
		AddChild( button );
		m_buttons.push_back( button );
		if ( item.submenu ) {
			item.submenu->SetLeft( GetWidth() + GetLeft() );
		}
		top += m_item_height;
	}

	On(
		UIEvent::EV_MOUSE_DOWN, EH() {
			return true; // prevent clickthrough
		}
	);

	if ( m_config.use_slide_animation ) {
		SetHeight( 0 );
	}
	else {
		SetHeight( CalculateHeight() );
	}
}

void Menu::Iterate() {
	UI::Iterate();

	if ( m_config.use_slide_animation ) {
		if ( !m_slide.IsRunning() && m_is_closing ) {
			// slidedown finished
			UI::Hide();
		}
		while ( m_slide.HasTicked() ) {
			SetHeight( m_slide.GetPosition() );
		}
	}
}

void Menu::Destroy() {

	Log( "Destroying side menu" );

	UI::Hide();

	RemoveChild( m_background );
	RemoveChild( m_frames.top );
	RemoveChild( m_frames.bottom );
	for ( auto& button : m_buttons ) {
		RemoveChild( button );
	}
	m_buttons.clear();

	UI::Destroy();
}

void Menu::ProcessEvent( event::UIEvent* event ) {
	if ( !m_slide.IsRunning() ) { // ignore events during slide
		UI::ProcessEvent( event );
	}
}

void Menu::Show() {
	if ( m_config.use_slide_animation ) {
		// slide up
		m_is_closing = false;
		m_slide.Scroll( GetHeight(), CalculateHeight() );
	}

	UI::Show();
}

void Menu::Hide() {

	if ( m_active_button ) {
		m_active_button->RemoveStyleModifier( Style::M_SELECTED );
		m_active_button = nullptr;
	}
	if ( m_active_submenu ) {
		m_active_submenu->Hide();
		m_active_submenu = nullptr;
	}

	if ( m_config.use_slide_animation ) {
		// slide down
		m_is_closing = true;
		m_slide.Scroll( GetHeight(), 0 );
	}
	else {
		UI::Hide();
	}
}

void Menu::AddItem( const std::string& label, std::function< bool( LabelButton* button, menu_item_t item ) > on_click ) {
	ASSERT( !m_created, "don't add items to active menu" );
	m_menu_items.push_back(
		{
			label,
			nullptr,
			on_click
		}
	);
}

void Menu::AddSubMenu( const std::string& label, Menu* submenu ) {
	ASSERT( !m_created, "don't add submenus to active menu" );
	m_menu_items.push_back(
		{
			label,
			submenu,
			0
		}
	);
	g_engine->GetUI()->AddObject( submenu );
}

const size_t Menu::CalculateHeight() const {
	return m_margin * 2 + m_item_height * m_menu_items.size();
}

}
}
}
