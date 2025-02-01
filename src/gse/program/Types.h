#pragma once

#include <cstdint>

namespace gse {
namespace program {

enum variable_hints_t : uint8_t {
	VH_NONE,
	VH_CREATE_VAR,
	VH_CREATE_CONST,
};

enum operator_type_t {
	OT_NOOP,
	OT_RETURN,
	OT_BREAK,
	OT_CONTINUE,
	OT_THROW,
	OT_ASSIGN,
	OT_NOT,
	OT_EQ,
	OT_NE,
	OT_LT,
	OT_LTE,
	OT_GT,
	OT_GTE,
	OT_AND,
	OT_OR,
	OT_ADD,
	OT_SUB,
	OT_MULT,
	OT_DIV,
	OT_MOD,
	OT_INC,
	OT_DEC,
	OT_INC_BY,
	OT_DEC_BY,
	OT_MULT_BY,
	OT_DIV_BY,
	OT_MOD_BY,
	OT_CHILD,
	OT_AT,
	OT_PUSH,
	OT_POP,
	OT_ERASE,
	OT_RANGE,
};

enum loop_control_type_t {
	LCT_NONE,
	LCT_BREAK,
	LCT_CONTINUE,
};

}
}
