#include "CenterAreaPage.h"

#include "CenterArea.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {
namespace center_area {

CenterAreaPage::CenterAreaPage( CenterArea* center_area )
	: BPSection( center_area->GetPopup(), "CenterAreaPage" )
	, m_center_area( center_area ) {
	m_config.no_outer_border = true;
	m_config.no_inner_border = true;
}

}
}
}
}
}
}
