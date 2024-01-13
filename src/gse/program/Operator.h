#pragma once

#include <unordered_map>

#include "Element.h"

namespace gse {
namespace program {

class Operator : public Element {
public:
	enum operator_type_t {
		OT_NOOP,
		OT_RETURN,
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
		OT_APPEND,
		OT_RANGE,
	};

	Operator( const si_t& si, const operator_type_t op )
		: Element( si, ET_OPERATOR )
		, op( op ) {}

	const operator_type_t op;

	const std::string ToString() const override {
		const auto it = m_op_labels.find( op );
		ASSERT_NOLOG( it != m_op_labels.end(), "op label not found: " + std::to_string( op ) );
		return it->second;
	}
	const std::string Dump( const size_t depth = 0 ) const override {
		const auto it = m_op_labels.find( op );
		ASSERT_NOLOG( it != m_op_labels.end(), "op label not found: " + std::to_string( op ) );
		return Formatted( "Operator" + m_si.ToString() + "( " + it->second + " )", depth );
	}

private:
	const std::unordered_map< operator_type_t, std::string > m_op_labels = {
		{
			OT_NOOP,
			""
		},
		{
			OT_RETURN,
			"return"
		},
		{
			OT_THROW,
			"throw"
		},
		{
			OT_ASSIGN,
			"="
		},
		{
			OT_NOT,
			"!"
		},
		{
			OT_EQ,
			"=="
		},
		{
			OT_NE,
			"!="
		},
		{
			OT_LT,
			"<"
		},
		{
			OT_LTE,
			"<="
		},
		{
			OT_GT,
			">"
		},
		{
			OT_GTE,
			">="
		},
		{
			OT_AND,
			"&&"
		},
		{
			OT_OR,
			"||"
		},
		{
			OT_ADD,
			"+"
		},
		{
			OT_SUB,
			"-"
		},
		{
			OT_MULT,
			"*"
		},
		{
			OT_DIV,
			"/"
		},
		{
			OT_MOD,
			"%"
		},
		{
			OT_INC,
			"++"
		},
		{
			OT_DEC,
			"--"
		},
		{
			OT_INC_BY,
			"+="
		},
		{
			OT_DEC_BY,
			"-="
		},
		{
			OT_MULT_BY,
			"*="
		},
		{
			OT_DIV_BY,
			"/="
		},
		{
			OT_MOD_BY,
			"%="
		},
		{
			OT_CHILD,
			"."
		},
		{
			OT_AT,
			"["
		},
		{
			OT_APPEND,
			"[]="
		},
		{
			OT_RANGE,
			"["
		},
	};
};

}
}

