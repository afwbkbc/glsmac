#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Parser.h"

#include "gse/program/Variable.h"
#include "gse/program/Array.h"
#include "gse/program/Conditional.h"

#include "gse/Value.h"
#include "gse/type/Bool.h"
#include "gse/type/Null.h"

namespace gse {
namespace parser {

CLASS( GJS, Parser )

	GJS( const std::string& source );

protected:
	void GetElements( source_elements_t& elements ) override;
	const program::Program* GetProgram( const source_elements_t& elements ) override;

private:

	const program::Scope* GetScope( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Control* GetControl( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Conditional* GetConditional( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Statement* GetStatement( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Operand* GetExpressionOrOperand( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end, bool is_scope_expected = false );
	const program::Expression* GetExpression( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end, const bool is_scope_expected = false );
	const program::Operand* GetOperand( const Identifier* element, program::Variable::variable_hints_t* next_var_hints = nullptr );
	const program::Operator* GetOperator( const Operator* element );
	const program::Array* GetArray( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Object* GetObject( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );

	const std::string CHARS_WHITESPACE = CHARS_EOLN + "	 ";
	const std::string CHARS_NAMES = CHARS_LETTERS + "_";
	const std::string CHARS_NAMES_C = CHARS_NAMES + CHARS_NUMBERS;
	const std::string CHARS_OPERATORS = "=+-:<>*/.!&|:";
	const std::string CHARS_OPERATORS_C = CHARS_OPERATORS + ".";
	const std::string CHARS_QUOTES = "'";
	const std::string CHARS_DELIMITERS = ";,{}()[]";

	enum identifier_type_t : uint8_t {
		IDENTIFIER_VARIABLE,
		IDENTIFIER_NUMBER,
		IDENTIFIER_STRING,
	};

	enum block_type_t : uint8_t {
		BLOCK_ROUND_BRACKETS,
		BLOCK_SQUARE_BRACKETS,
		BLOCK_CURLY_BRACKETS,
	};

	const std::unordered_set< std::string > KEYWORDS = {
		"return",
		"let",
		"true",
		"false",
		"null",
	};

	const std::unordered_map< std::string, Parser::Conditional::conditional_type_t > CONTROL_KEYWORDS = {
		{
			"if",
			Parser::Conditional::CT_IF
		},
		{
			"elseif",
			Parser::Conditional::CT_ELSEIF
		},
		{
			"else",
			Parser::Conditional::CT_ELSE
		},
		{
			"while",
			Parser::Conditional::CT_WHILE
		},
	};

	const std::unordered_map< std::string, program::Variable::variable_hints_t > MODIFIER_OPERATORS = {
		{
			{
				"let",
				program::Variable::VH_CREATE,
			}
		}
	};

	const std::unordered_map< std::string, gse::Value > PREDEF_OPERATORS = {
		{
			"true",
			VALUE( type::Bool, true ),
		},
		{
			"false",
			VALUE( type::Bool, false ),
		},
		{
			"null",
			VALUE( type::Null ),
		}
	};

	const std::unordered_map< std::string, program::Operator::operator_type_t > OPERATOR_NAMES = {
		{
			"return",
			program::Operator::OT_RETURN
		},
		{
			"=",
			program::Operator::OT_ASSIGN
		},
		{
			"!",
			program::Operator::OT_NOT
		},
		{
			"==",
			program::Operator::OT_EQ
		},
		{
			"!=",
			program::Operator::OT_NE
		},
		{
			"<",
			program::Operator::OT_LT
		},
		{
			"<=",
			program::Operator::OT_LTE
		},
		{
			">",
			program::Operator::OT_GT
		},
		{
			">=",
			program::Operator::OT_GTE
		},
		{
			"&&",
			program::Operator::OT_AND
		},
		{
			"||",
			program::Operator::OT_OR
		},
		{
			"+",
			program::Operator::OT_ADD
		},
		{
			"-",
			program::Operator::OT_SUB
		},
		{
			"*",
			program::Operator::OT_MULT
		},
		{
			"/",
			program::Operator::OT_DIV
		},
		{
			"%",
			program::Operator::OT_MOD
		},
		{
			"++",
			program::Operator::OT_INC
		},
		{
			"--",
			program::Operator::OT_DEC
		},
		{
			"+=",
			program::Operator::OT_INC_BY
		},
		{
			"-=",
			program::Operator::OT_DEC_BY
		},
		{
			"*=",
			program::Operator::OT_MULT_BY
		},
		{
			"/=",
			program::Operator::OT_DIV_BY
		},
		{
			"%/",
			program::Operator::OT_MOD_BY
		},
		{
			".",
			program::Operator::OT_CHILD
		},
		{
			"[]",
			program::Operator::OT_AT
		},
		{
			"[]=",
			program::Operator::OT_APPEND
		},
		{
			":",
			program::Operator::OT_RANGE
		},
	};

	enum operator_link_t {
		OL_LEFT,
		OL_RIGHT,
		OL_ANY,
		OL_BOTH,
		OL_ANY_OR_BOTH,
	};
	struct operator_info_t {
		uint8_t priority;
		operator_link_t link;
	};
	const std::unordered_map< const program::Operator::operator_type_t, const operator_info_t > OPERATOR_INFO = {
		{
			program::Operator::OT_RETURN,
			{
				1,
				OL_RIGHT
			}
		},
		{
			program::Operator::OT_ASSIGN,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_NOT,
			{
				14,
				OL_RIGHT
			}
		},
		{
			program::Operator::OT_EQ,
			{
				8,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_NE,
			{
				8,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_LT,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_LTE,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_GT,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_GTE,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_AND,
			{
				4,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_OR,
			{
				3,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_ADD,
			{
				11,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_SUB,
			{
				11,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_MULT,
			{
				12,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_DIV,
			{
				12,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_MOD,
			{
				12,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_INC,
			{
				15,
				OL_ANY
			}
		},
		{
			program::Operator::OT_DEC,
			{
				15,
				OL_ANY
			}
		},
		{
			program::Operator::OT_INC_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_DEC_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_MULT_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_DIV_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_MOD_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_CHILD,
			{
				17,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_AT,
			{
				18,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_APPEND,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::Operator::OT_RANGE,
			{
				3,
				OL_ANY_OR_BOTH
			}
		}
	};

	const source_elements_t::const_iterator GetBracketsEnd( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );

#ifdef DEBUG
	void LogElement( const std::string& prefix, const SourceElement* element ) const;
	void LogElements( const std::string& label, const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const;
#endif

};

}
}
