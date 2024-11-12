#include "Module.h"

#include "game/backend/Random.h"
#include "game/backend/map/Map.h"

namespace game {
namespace backend {
namespace map {
namespace module {

Module::Module( Map* map )
	: m_map( map ) {

}

const uint8_t Module::RandomRotate() const {
	return m_map->GetRandom()->GetUInt( 0, 3 );
}

}
}
}
}
