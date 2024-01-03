#pragma once

#include <stdexcept>
#include <vector>

#include "program/Element.h"

namespace gse {

class Context;

class Exception : public std::runtime_error {
public:

	typedef std::vector< std::string > backtrace_t;

	Exception( const std::string& class_name, const std::string& reason, const gse::Context* context, const si_t& si )
		: std::runtime_error( "[" + class_name + "] " + reason )
		, class_name( class_name )
		, reason( reason )
		, context( context )
		, si( si ) {}

	const std::string class_name;
	const std::string reason;
	const Context* context;
	const si_t si;

	bool contexts_freed = false;

	const backtrace_t GetBacktraceAndCleanup( const Context* const current_ctx );

};

}

#include "Context.h"