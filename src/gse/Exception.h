#pragma once

#include <stdexcept>
#include <vector>

#include "program/Element.h"

namespace gse {

class Context;

struct exception_ec_t {
	const std::string PARSE_ERROR;
	const std::string VAR_NOT_DEFINED;
	const std::string INVALID_ASSIGNMENT;
	const std::string OPERATOR_NOT_SUPPORTED;
	const std::string OPERATION_FAILED;
};
extern const exception_ec_t EC;

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

	const std::string ToStringAndCleanup();

};

}

#include "Context.h"