#pragma once

#include <vector>

#include "types/Exception.h"
#include "program/Element.h"

namespace gse {

class Context;

struct exception_ec_t {
	const std::string PARSE_ERROR;
	const std::string TYPE_ERROR;
	const std::string REFERENCE_ERROR;
	const std::string INVALID_ASSIGNMENT;
	const std::string OPERATION_NOT_SUPPORTED;
	const std::string OPERATION_FAILED;
	const std::string INVALID_DEREFERENCE;
	const std::string INVALID_CALL;
	const std::string LOADER_ERROR;
	const std::string MATH_ERROR;
	const std::string CONVERSION_ERROR;
	const std::string GAME_ERROR;
	const std::string MAP_ERROR;
	const std::string INVALID_EVENT;
};
extern const exception_ec_t EC;

class Exception : public types::Exception {
public:

	typedef std::vector< std::string > backtrace_t;

	Exception( const std::string& class_name, const std::string& reason, gse::Context* context, const si_t& si )
		: types::Exception( class_name, reason )
		, class_name( class_name )
		, reason( reason )
		, context( context )
		, si( si ) {}

	const std::string class_name;
	const std::string reason;
	Context* context;
	const si_t si;

	bool contexts_freed = false;

	const backtrace_t GetBacktraceAndCleanup( const Context* const current_ctx );

	const std::string ToStringAndCleanup();

};

}

#include "ChildContext.h"
