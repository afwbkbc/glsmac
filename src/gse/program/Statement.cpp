#include "Statement.h"

#include "Expression.h"

namespace gse {
namespace program {

Statement::Statement( const si_t& si, const Expression* body )
	: Control( si, CT_STATEMENT )
	, body( body ) {}

Statement::~Statement() {
	delete body;
}

const std::string Statement::Dump( const size_t depth ) const {
	return Formatted( "Statement" + m_si.ToString() + "(", depth ) +
		body->Dump( depth + 1 ) +
		Formatted( ")", depth );
}

}
}
