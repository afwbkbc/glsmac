#include "Module.h"

namespace game {
namespace world {
namespace map {
	
Module::Module( Map* map )
	: m_map( map )
{
	
}

const uint8_t Module::RandomRotate() const {
	return m_map->GetRandom()->GetUInt( 0, 3 );
}

}
}
}
