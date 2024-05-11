#include "Object.h"

#include "Expression.h"

namespace gse {
namespace program {

Object::Object( const si_t& si, const properties_t& properties )
	: Operand( si, OT_OBJECT )
	, properties( properties ) {}

Object::~Object() {
	for ( auto& it : properties ) {
		delete it.second;
	}
}

const std::string Object::ToString() const {
	std::string args = "";
	for ( const auto& it : properties ) {
		if ( !args.empty() ) {
			args += ", ";
		}
		args += it.first + ": " + it.second->ToString();
	}
	return "{ " + args + " }";
}
const std::string Object::Dump( const size_t depth ) const {
	std::string result = Formatted( "Object" + m_si.ToString() + "( ", depth );
	for ( const auto& it : properties ) {
		result +=
			Formatted( it.first + ":", depth + 1 ) +
				it.second->Dump( depth + 2 );
	}
	return result +
		Formatted( ")", depth );
}

}
}
