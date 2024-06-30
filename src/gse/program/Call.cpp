#include "Call.h"

#include "Expression.h"

namespace gse {
namespace program {

Call::Call( const si_t& si, const Expression* callable, const std::vector< const Expression* >& arguments )
	: Operand( si, OT_CALL )
	, callable( callable )
	, arguments( arguments ) {}

Call::~Call() {
	delete callable;
	for ( auto& it : arguments ) {
		delete it;
	}
}

const std::string Call::ToString() const {
	std::string args = "";
	for ( const auto& it : arguments ) {
		if ( !args.empty() ) {
			args += ", ";
		}
		args += it->ToString();
	}
	return callable->ToString() + "( " + args + " )";
}

const std::string Call::Dump( const size_t depth ) const {
	std::string args = "";
	for ( const auto& it : arguments ) {
		args += it->Dump( depth + 2 );
	}
	return Formatted( "Call" + m_si.ToString() + "(", depth ) +
		Formatted( "Callable(", depth + 1 ) +
		callable->Dump( depth + 2 ) +
		Formatted( ")", depth + 1 ) +
		Formatted( "Arguments(", depth + 1 ) +
		args +
		Formatted( ")", depth + 1 ) +
		Formatted( ")", depth );

}

}
}
