#include "Menu.h"

#include "engine/Engine.h"
#include "game/frontend/Game.h"
#include "ui_legacy/UI.h"
#include "ui_legacy/object/Surface.h"
#include "ui_legacy/object/LabelButton.h"

namespace game {
namespace frontend {
namespace ui_legacy {

Menu::Menu( Game* game, const std::string& class_name, const uint8_t item_height, const uint8_t margin )
	: UI( game, class_name )
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

	NEW( m_background, ::ui_legacy::object::Surface, "BBMenuBackground" );
	AddChild( m_background );

	NEW( m_frames.top, ::ui_legacy::object::Surface, SubClass( "TopFrame" ) );
	AddChild( m_frames.top );

	NEW( m_frames.bottom, ::ui_legacy::object::Surface, "BBMenuBottomFrame" );
	AddChild( m_frames.bottom );

	size_t top = m_margin;
	for ( auto& item : m_menu_items ) {
		NEWV( button, ::ui_legacy::object::LabelButton, SubClass( "Button" ) );
		button->SetHeight( m_item_height );
		button->SetTop( top );
		button->SetLabel( item.label );
		button->SetAreaLimitsByObject( this );
		button->On(
			::ui_legacy::event::EV_BUTTON_CLICK, EH( this, item, button ) {
				if ( m_active_button && m_active_button != button ) {
					m_active_button->RemoveStyleModifier( ::ui_legacy::M_SELECTED );
					m_active_button = nullptr;
				}
				if ( m_active_submenu && m_active_submenu != item.submenu ) {
					m_active_submenu->Hide();
					m_active_submenu = nullptr;
				}
				if ( item.submenu ) {
					if ( button->HasStyleModifier( ::ui_legacy::M_SELECTED ) ) {
						button->RemoveStyleModifier( ::ui_legacy::M_SELECTED );
						item.submenu->Hide();
						m_active_button = nullptr;
						m_active_submenu = nullptr;
					}
					else {
						m_active_button = button;
						m_active_button->AddStyleModifier( ::ui_legacy::M_SELECTED );
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
		::ui_legacy::event::EV_MOUSE_DOWN, EH() {
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

void Menu::ProcessEvent( ::ui_legacy::event::UIEvent* event ) {
	if ( !m_slide.IsRunning() ) { // ignore events during slide
		UI::ProcessEvent( event );
	}
}

void Menu::Show() {
	if ( m_config.use_slide_animation ) {
		// slide up
		m_is_closing = false;
		m_slide.Scroll( GetHeight(), CalculateHeight(), SLIDE_DURATION_MS );
	}

	UI::Show();
}

void Menu::Hide() {

	if ( m_active_button ) {
		m_active_button->RemoveStyleModifier( ::ui_legacy::M_SELECTED );
		m_active_button = nullptr;
	}
	if ( m_active_submenu ) {
		m_active_submenu->Hide();
		m_active_submenu = nullptr;
	}

	if ( m_config.use_slide_animation ) {
		// slide down
		m_is_closing = true;
		m_slide.Scroll( GetHeight(), 0, SLIDE_DURATION_MS );
	}
	else {
		UI::Hide();
	}
}

void Menu::AddItem( const std::string& label, std::function< bool( ::ui_legacy::object::LabelButton* button, menu_item_t item ) > on_click ) {
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
