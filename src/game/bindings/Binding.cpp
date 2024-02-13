#include "Binding.h"

namespace game {
namespace bindings {

Binding::Binding( const std::string& name, Bindings* bindings )
	: m_name( name )
	, m_bindings( bindings )
	, m_game( bindings->GetGame() ) {
	//
}

void Binding::Add( gse::type::Object::properties_t& methods ) {
	methods.insert_or_assign( m_name, Get() );
}

}
}
