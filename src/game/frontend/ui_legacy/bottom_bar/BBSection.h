#pragma once

#include "game/frontend/ui_legacy/Section.h"

namespace game {
namespace frontend {
namespace ui_legacy {

CLASS( BBSection, Section )

	BBSection( Game* game, const std::string class_name = "" )
		: Section( game, "BB" + class_name, "BB" ) {}

};

}
}
}
