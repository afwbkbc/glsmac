#pragma once

#include "BBSection.h"
#include "ui/object/Section.h"

namespace task {
namespace game {
namespace ui {

CLASS( UnitsList, BBSection )

	UnitsList( Game* game )
		: BBSection( game, "UnitsList" ) {}

	void Create() override;
	void Destroy() override;

};

}
}
}
