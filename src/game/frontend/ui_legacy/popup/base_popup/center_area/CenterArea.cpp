#include "CenterArea.h"

#include "ui_legacy/object/Button.h"

#include "Resource.h"
#include "Support.h"
#include "Psych.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace center_area {

CenterArea::CenterArea( BasePopup* popup )
	: BPSection( popup, "CenterArea" ) {}

void CenterArea::Create() {
	BPSection::Create();

#define X( _b, _l, _t, _c ) \
    NEW( m_buttons._b, ::ui_legacy::object::Button, "PopupButtonCenterArea" ); \
    m_buttons._b->SetLeft( _l ); \
    m_buttons._b->SetLabel( _t ); \
    m_buttons._b->On( ::ui_legacy::event::EV_BUTTON_CLICK, EH( this ) { \
        ActivateButton( m_buttons._b ); \
        return true; \
    } ); \
    AddChild( m_buttons._b ); \
    NEW( m_pages._b, _c, this ); \
    AddChild( m_pages._b );
	X( resource, 3, "RESOURCE", Resource )
	X( support, 133, "SUPPORT", Support )
	X( psych, 263, "PSYCH", Psych )
#undef X

	ActivateButton( m_buttons.resource );
}

void CenterArea::Destroy() {
#define X( _b ) \
    RemoveChild( m_buttons._b ); \
    RemoveChild( m_pages._b );
	X( resource )
	X( support )
	X( psych )
#undef X
	BPSection::Destroy();
}

BasePopup* CenterArea::GetPopup() const {
	return m_popup;
}

void CenterArea::ActivateButton( ::ui_legacy::object::Button* button ) {
#define X( _b, _c ) \
    if ( m_buttons._b == button ) { \
        m_buttons._b->AddStyleModifier( ::ui_legacy::M_SELECTED ); \
        m_pages._b->Show(); \
    } \
    else { \
        m_buttons._b->RemoveStyleModifier( ::ui_legacy::M_SELECTED ); \
        m_pages._b->Hide(); \
    }
	X( resource, Resource )
	X( support, Support )
	X( psych, Psych )
#undef X
}

void CenterArea::Update( base::Base* base ) {
	m_pages.resource->Update( base );
}

}
}
}
}
}
}
