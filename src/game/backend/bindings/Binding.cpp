#include "Binding.h"

#include "game/backend/State.h"

namespace game {
namespace backend {
namespace bindings {

Binding::Binding( const std::string& name, Bindings* bindings )
	: m_name( name )
	, m_bindings( bindings ) {
	//
}

void Binding::Add( gse::type::object_properties_t& methods ) {
	methods.insert_or_assign( m_name, Get() );
}

}
}
}
