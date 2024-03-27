#include "Morale.h"

namespace game {
namespace unit {

// must be hardcoded because unit badge sprites are
const Morale::morale_t Morale::MORALE_MIN = 0;
const Morale::morale_t Morale::MORALE_MAX = 6;

Morale::Morale( const std::string& name )
	: m_name( name ) {

}

}
}