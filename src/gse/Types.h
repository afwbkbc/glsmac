#pragma once

#include <string>
#include <cstdlib>
#include <cstdint>

#include "value/Types.h"

namespace gse {

namespace context {
class Context;
}

// source info, this is how source elements (and later programs) are linked to source code
struct si_t {
	std::string file = "";
	struct pos_t {
		size_t line = 0;
		size_t col = 0;
	};
	pos_t from = {};
	pos_t to = {};
	const std::string ToString() const {
		return "[" + file + ":" + std::to_string( from.line ) + ":" + std::to_string( from.col ) + "]";
	}
};

class ExecutionPointer;

enum value_type_t : uint8_t {
	VT_NULLPTR, // special type only for serialization/deserialization
	VT_NOTHING, // special type meaning 'no type'
	VT_UNDEFINED,
	VT_NULL,
	VT_BOOL,
	VT_INT,
	VT_FLOAT,
	VT_STRING,
	VT_ARRAY,
	VT_OBJECT,
	VT_CALLABLE,
	VT_ARRAYREF,
	VT_ARRAYRANGEREF,
	VT_OBJECTREF,
	VT_VALUEREF,
	VT_RANGE,
	VT_LOOPCONTROL,
};

}
