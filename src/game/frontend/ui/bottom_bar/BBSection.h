#pragma once

#include "game/frontend/ui/Section.h"

namespace game {
namespace frontend {
namespace ui {

CLASS( BBSection, Section )

	BBSection( Game* game, const std::string class_name = "" )
		: Section( game, "BB" + class_name, "BB" ) {}

};

}
}
}
