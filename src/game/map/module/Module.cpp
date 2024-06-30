#include "Module.h"

#include "util/random/Random.h"
#include "game/map/Map.h"

namespace game {
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
