#include "BPSection.h"

#include "BasePopup.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {

BPSection::BPSection( BasePopup* popup, const std::string class_name )
	: Section( popup->GetGame(), "BP" + class_name, "BB" )
	, m_popup( popup ) {
	m_config.no_outer_border = true;
	m_config.no_inner_background = true;
}

}
}
}
}
}
