#pragma once

#include "CenterAreaPage.h"

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace ui {
namespace popup {
namespace base_popup {
namespace center_area {

CLASS( Resource, CenterAreaPage )

	Resource( CenterArea* center_area );

private:
	friend class CenterArea;
	void Update( base::Base* base );

};

}
}
}
}
}
}
