#include "PopupMenu.h"

#include "engine/Engine.h"

#include "ui/object/Section.h"
#include "ui/object/Panel.h"
#include "ui/object/Button.h"
#include "ui/UI.h"

namespace task {
namespace mainmenu {

PopupMenu::PopupMenu( MainMenu* mainmenu, const std::string& title )
	: MenuObject( mainmenu, title ) {

}

const bool PopupMenu::IsShown() const {
	return m_is_shown;
}

void PopupMenu::Show() {

	ASSERT( !m_is_shown, "popupmenu already shown" );
	ASSERT( m_width > 0, "popupmenu width is zero" );
	ASSERT( m_height > 0, "popupmenu height is zero" );

	auto on_ok = [ this ]() -> void {
		OnNext();
	};
	auto on_cancel = [ this ]() -> void {
		if ( OnCancel() ) {
			GoBack();
		}
	};

	NEW( m_frame, ui::object::Section, "PopupWindow" );
	m_frame->SetTitleText( m_title );
	m_frame->SetAlign( ui::ALIGN_CENTER );
	Resize();
	m_frame->On(
		ui::event::EV_KEY_DOWN, EH( this, on_ok, on_cancel ) {
			if ( !data->key.modifiers ) {
				if ( data->key.code == ui::event::K_ENTER ) {
					if ( HasFlag( PF_HAS_OK ) ) {
						on_ok();
						return true;
					}
					/*else if ( HasFlag( PF_HAS_CANCEL ) ) {
						on_cancel();
						return true;
					}*/
				}
				else if ( data->key.code == ui::event::K_ESCAPE ) {
					// TMP
					//if ( HasFlag( PF_HAS_CANCEL ) || HasFlag( PF_HAS_OK ) ) {
					on_cancel();
					return true;
					//}
				}
			}
			return false;
		}
	);
	g_engine->GetUI()->AddObject( m_frame );

	NEW( m_body, ui::object::Panel );
	m_body->SetAlign( ui::ALIGN_TOP );
	if ( HasFlag( PF_HAS_OK ) || HasFlag( PF_HAS_CANCEL ) ) {
		m_body->SetBottom( 40 );
	}
	m_frame->AddChild( m_body );

	if ( HasFlag( PF_HAS_OK ) ) {
		NEW( m_button_ok, ui::object::Button, "PopupButtonOkCancel" );
		if ( HasFlag( PF_HAS_CANCEL ) ) {
			// two buttons
			m_button_ok->SetAlign( ui::ALIGN_BOTTOM | ui::ALIGN_LEFT );
			m_button_ok->SetLeft( 12 );
		}
		else {
			// one button
			m_button_ok->SetAlign( ui::ALIGN_BOTTOM | ui::ALIGN_HCENTER );
		}
		m_button_ok->SetBottom( 15 );
		m_button_ok->SetLabel( "OK" );
		m_button_ok->On(
			ui::event::EV_BUTTON_CLICK, EH( this, on_ok ) {
				on_ok();
				return true;
			}
		);
		m_frame->AddChild( m_button_ok );
	}

	if ( HasFlag( PF_HAS_CANCEL ) ) {
		NEW( m_button_cancel, ui::object::Button, "PopupButtonOkCancel" );
		if ( HasFlag( PF_HAS_OK ) ) {
			// two buttons
			m_button_cancel->SetAlign( ui::ALIGN_BOTTOM | ui::ALIGN_RIGHT );
			m_button_cancel->SetRight( 12 );
		}
		else {
			// one button
			m_button_cancel->SetAlign( ui::ALIGN_BOTTOM | ui::ALIGN_HCENTER );
		}
		m_button_cancel->SetBottom( 15 );
		m_button_cancel->SetLabel( "CANCEL" );
		m_button_cancel->On(
			ui::event::EV_BUTTON_CLICK, EH( this, on_cancel ) {
				on_cancel();
				return true;
			}
		);
		m_frame->AddChild( m_button_cancel );
	}

	m_is_shown = true;
}

void PopupMenu::Hide() {

	ASSERT( m_is_shown, "popupmenu already hidden" );

	m_frame->RemoveChild( m_body );

	if ( HasFlag( PF_HAS_OK ) ) {
		m_frame->RemoveChild( m_button_ok );
	}

	if ( HasFlag( PF_HAS_CANCEL ) ) {
		m_frame->RemoveChild( m_button_cancel );
	}

	g_engine->GetUI()->RemoveObject( m_frame );

	m_is_shown = false;
}

void PopupMenu::SetFlags( const std::unordered_set< popup_flag_t > flags ) {
	m_flags = flags;
}

void PopupMenu::SetWidth( const size_t width ) {
	m_width = width;
}

void PopupMenu::SetHeight( const size_t height ) {
	m_height = height;
}

bool PopupMenu::HasFlag( const popup_flag_t flag ) const {
	return m_flags.find( flag ) != m_flags.end();
}

void PopupMenu::Resize() {
	if ( m_frame ) {
		m_frame->SetWidth( m_width );
		m_frame->SetHeight( m_height );
	}
}

}
}
