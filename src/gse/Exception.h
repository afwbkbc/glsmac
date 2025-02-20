#pragma once

#include <vector>

#include "types/Exception.h"

#include "gse/Types.h"
#include "Value.h"

namespace gse {

namespace context {
class Context;
}

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
	const std::string INVALID_DEFINITION;
	const std::string INVALID_HANDLER;
	const std::string UI_ERROR;
};
extern const exception_ec_t EC;

class Exception : public types::Exception {
public:

	typedef std::vector< std::string > stacktrace_t;

	Exception( const std::string& class_name, const std::string& reason, GSE_CALLABLE_NOGC );

	const std::string class_name;
	const std::string reason;

	const stacktrace_t& GetStackTrace() const;
	const std::string ToString() const;

private:
	stacktrace_t m_stacktrace = {};

};

}
