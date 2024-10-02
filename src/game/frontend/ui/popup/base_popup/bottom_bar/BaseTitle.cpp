#include "BaseTitle.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {
namespace bottom_bar {

BaseTitle::BaseTitle( Game* game )
	: BBSection( game, "BaseTitle" ) {
	//
}

void BaseTitle::Create() {
	BBSection::Create();

}

void BaseTitle::Destroy() {

	BBSection::Destroy();
}

}
}
}
}
}
}
