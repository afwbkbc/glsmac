#include "Def.h"

namespace game {
namespace unit {

Def::Def( const std::string& name )
	: m_name( name ) {
	//
}

const std::string& Def::GetName() const {
	return m_name;
}

}
}