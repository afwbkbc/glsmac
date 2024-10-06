#include "Population.h"

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {
namespace bottom_bar {

Population::Population( Game* game )
	: BBSection( game, "Population" ) {
	m_config.no_inner_border = true;
}

void Population::Create() {
	BBSection::Create();

}

void Population::Destroy() {

	BBSection::Destroy();
}

}
}
}
}
}
}
