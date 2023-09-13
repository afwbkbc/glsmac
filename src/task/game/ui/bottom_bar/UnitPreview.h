#pragma once

#include "BBSection.h"
#include "ui/object/Section.h"

namespace task {
namespace game {
namespace ui {

CLASS( UnitPreview, BBSection )

	UnitPreview( Game* game ) : BBSection( game, "UnitPreview" ) {}
	
	void Create() override;
	void Destroy() override;

};
	
}
}
}
