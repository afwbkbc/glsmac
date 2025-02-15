#include "ExecutionPointer.h"

namespace gse {

void ExecutionPointer::WithSI( const si_t& si, const std::function< void() >& f ) {
	m_stacktrace.push_back( si );
	try {
		f();
	}
	catch ( const std::exception& e ) {
		m_stacktrace.pop_back();
		throw;
	}
	m_stacktrace.pop_back();
}

const std::vector< si_t > ExecutionPointer::GetStackTrace() const {
	return m_stacktrace;
}

}
