#pragma once

#include "game/frontend/ui_legacy/popup/base_popup/BPSection.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace center_area {

class CenterArea;

CLASS( CenterAreaPage, BPSection )

	CenterAreaPage( CenterArea* center_area );

protected:
	CenterArea* const m_center_area;

private:

};

}
}
}
}
}
}
