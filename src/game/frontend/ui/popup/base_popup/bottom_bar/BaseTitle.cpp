#include "BaseTitle.h"

#include "ui/object/Label.h"

#include "game/frontend/ui/popup/base_popup/BasePopup.h"
#include "game/frontend/base/Base.h"
#include "game/frontend/faction/Faction.h"
#include "util/String.h"

namespace game {
namespace frontend {
namespace ui {
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

	NEW( m_label, ::ui::object::Label, SubClass( "Text" ) );
	m_label->SetText( util::String::ToUpperCase( m_popup->GetBase()->GetName() ) );
	m_label->SetTextColor( m_popup->GetBase()->GetFaction()->m_colors.text );
	AddChild( m_label );
}

void BaseTitle::Destroy() {

	RemoveChild( m_label );

	BBSection::Destroy();
}

}
}
}
}
}
}
