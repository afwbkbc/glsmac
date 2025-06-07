#include "Object.h"

#include "common/Assert.h"

#include "Expression.h"

namespace gse {
namespace program {

Object::Object( const si_t& si, const ordered_properties_t& ordered_properties )
	: Operand( si, OT_OBJECT )
	, ordered_properties( ordered_properties ) {
	for ( const auto& it : ordered_properties ) {
		ASSERT( properties.find( it.first ) == properties.end(), "duplicate property" );
		properties.insert( it );
	}
}

Object::~Object() {
	for ( auto& it : properties ) {
		delete it.second;
	}
}

const std::string Object::ToString() const {
	std::string args = "";
	for ( const auto& it : ordered_properties ) {
		if ( !args.empty() ) {
			args += ", ";
		}
		args += it.first + ": " + it.second->ToString();
	}
	return "{ " + args + " }";
}
const std::string Object::Dump( const size_t depth ) const {
	std::string result = Formatted( "Object" + m_si.ToString() + "( ", depth );
	for ( const auto& it : ordered_properties ) {
		result +=
			Formatted( it.first + ":", depth + 1 ) +
				it.second->Dump( depth + 2 );
	}
	return result +
		Formatted( ")", depth );
}

}
}
