#pragma once

#include <vector>
#include <functional>

#include "Types.h"

namespace gse {

class ExecutionPointer {
public:

	void WithSI( const si_t& si, const std::function< void() >& f );
	const std::vector< si_t > GetStackTrace() const;

private:

	std::vector< si_t > m_stacktrace = {};

};

}
