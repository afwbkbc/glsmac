#include "BaseTitle.h"

#include "ui_legacy/object/Label.h"
#include "ui_legacy/object/SimpleButton.h"

#include "game/frontend/ui_legacy/popup/base_popup/BasePopup.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/faction/Faction.h"
#include "util/String.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace bottom_bar {

BaseTitle::BaseTitle( Game* game, BasePopup* popup )
	: BBSection( game, "BaseTitle" )
	, m_popup( popup ) {
	//
}

void BaseTitle::Create() {
	BBSection::Create();

	NEW( m_label, ::ui_legacy::object::Label, SubClass( "Text" ) );
	AddChild( m_label );

	NEW( m_arrows.left, ::ui_legacy::object::SimpleButton, SubClass( "LeftArrow" ) );
	m_arrows.left->On(
		::ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			m_popup->SelectNextBase();
			return true;
		}
	);
	AddChild( m_arrows.left );

	NEW( m_arrows.right, ::ui_legacy::object::SimpleButton, SubClass( "RightArrow" ) );
	m_arrows.right->On(
		::ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			m_popup->SelectPrevBase();
			return true;
		}
	);
	AddChild( m_arrows.right );

}

void BaseTitle::Destroy() {

	RemoveChild( m_label );
	RemoveChild( m_arrows.left );
	RemoveChild( m_arrows.right );

	BBSection::Destroy();
}

void BaseTitle::Update( base::Base* base ) {
	m_label->SetText( util::String::ToUpperCase( base->GetName() ) );
	m_label->SetTextColor( base->GetFaction()->m_colors.text );
}

}
}
}
}
}
}
