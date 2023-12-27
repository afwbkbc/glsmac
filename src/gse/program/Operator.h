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

	Operator( const operator_type_t op )
		: Element( ET_OPERATOR )
		, op( op ) {}

	const operator_type_t op;

	const std::string ToString( const size_t depth = 0 ) const override {
		const auto it = m_op_labels.find( op );
		ASSERT_NOLOG( it != m_op_labels.end(), "op label not found: " + std::to_string( op ) );
		return Formatted( "Operator( " + it->second + " )", depth );
	}

private:
	const std::unordered_map< operator_type_t, std::string > m_op_labels = {
		{
			OT_NOOP,
			"NOOP"
		},
		{
			OT_RETURN,
			"RETURN"
		},
		{
			OT_ASSIGN,
			"ASSIGN"
		},
		{
			OT_NOT,
			"NOT"
		},
		{
			OT_EQ,
			"EQ"
		},
		{
			OT_NE,
			"NE"
		},
		{
			OT_LT,
			"LT"
		},
		{
			OT_LTE,
			"LTE"
		},
		{
			OT_GT,
			"GT"
		},
		{
			OT_GTE,
			"GTE"
		},
		{
			OT_AND,
			"AND"
		},
		{
			OT_OR,
			"OR"
		},
		{
			OT_ADD,
			"ADD"
		},
		{
			OT_SUB,
			"SUB"
		},
		{
			OT_MULT,
			"MULT"
		},
		{
			OT_DIV,
			"DIV"
		},
		{
			OT_MOD,
			"MOD"
		},
		{
			OT_INC,
			"INC"
		},
		{
			OT_DEC,
			"DEC"
		},
		{
			OT_INC_BY,
			"INC_BY"
		},
		{
			OT_DEC_BY,
			"DEC_BY"
		},
		{
			OT_MULT_BY,
			"MULT_BY"
		},
		{
			OT_DIV_BY,
			"DIV_BY"
		},
		{
			OT_MOD_BY,
			"MOD_BY"
		},
		{
			OT_CHILD,
			"CHILD"
		},
		{
			OT_AT,
			"AT"
		},
		{
			OT_APPEND,
			"APPEND"
		},
		{
			OT_RANGE,
			"RANGE"
		},
	};
};

}
}

