#pragma once

#include "../Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( BBSection, Section )

	BBSection( World* world, const std::string class_name = "" ) : Section( world, "BB" + class_name, "BB" ) {}

};
	
}
}
}
