#include "BuildQueue.h"

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace bottom_bar {

BuildQueue::BuildQueue( Game* game )
	: BBSection( game, "BuildQueue" ) {
	//
}

void BuildQueue::Create() {
	BBSection::Create();

}

void BuildQueue::Destroy() {

	BBSection::Destroy();
}

}
}
}
}
}
}
