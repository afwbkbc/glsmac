#pragma once

#include "task/game/ui/Section.h"

namespace task {
namespace game {
namespace ui {

CLASS( BBSection, Section )

	BBSection( Game* game, const std::string class_name = "" )
		: Section( game, "BB" + class_name, "BB" ) {}

};

}
}
}
