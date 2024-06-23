#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Parser.h"

#include "gse/program/Types.h"

#include "gse/Value.h"
#include "gse/type/Bool.h"
#include "gse/type/Null.h"
#include "gse/type/Undefined.h"

namespace gse {

namespace program {
class Scope;
class Statement;
class Control;
class Conditional;
class Expression;
class Operator;
class Operand;
class Object;
class Array;
}

namespace parser {

CLASS( JS, Parser )

	JS( const std::string& filename, const std::string& source, const size_t initial_line_num );

protected:
	void GetElements( source_elements_t& elements ) override;
	const program::Program* GetProgram( const source_elements_t& elements ) override;

private:

	const si_t GetSI( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );

	const program::Scope* GetScope( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Control* GetControl( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Conditional* GetConditional( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Statement* GetStatement( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Operand* GetExpressionOrOperand( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Expression* GetExpression( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Operand* GetOperand( const Identifier* element, program::variable_hints_t* next_var_hints = nullptr );
	const program::Operator* GetOperator( const Operator* element );
	const program::Array* GetArray( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );
	const program::Object* GetObject( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end );

	const std::string CHARS_WHITESPACE = CHARS_EOLN + "	 ";
	const std::string CHARS_NAMES = CHARS_LETTERS + "_#";
	const std::string CHARS_NAMES_C = CHARS_LETTERS + CHARS_NUMBERS + "_";
	const std::string CHARS_OPERATORS = "=+-:.<>*/!&|%";
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
	struct block_info_t {
		Block::block_info_t BLOCK_ROUND_BRACKETS;
		Block::block_info_t BLOCK_SQUARE_BRACKETS;
		Block::block_info_t BLOCK_CURLY_BRACKETS;
	} block_info = {
		{
			BLOCK_ROUND_BRACKETS,
			'(',
			')'
		},
		{
			BLOCK_SQUARE_BRACKETS,
			'[',
			']'
		},
		{
			BLOCK_CURLY_BRACKETS,
			'{',
			'}'
		},
	};

	const std::unordered_set< std::string > KEYWORDS = {
		"return",
		"throw",
		"let",
		"const",
		"true",
		"false",
		"null",
		"undefined",
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
		{
			"for",
			Parser::Conditional::CT_FOR
		},
		{
			"try",
			Parser::Conditional::CT_TRY
		},
		{
			"catch",
			Parser::Conditional::CT_CATCH
		}
	};

	const std::unordered_map< std::string, program::variable_hints_t > MODIFIER_OPERATORS = {
		{
			{
				"let",
				program::VH_CREATE_VAR,
			},
			{
				"const",
				program::VH_CREATE_CONST,
			},
		}
	};

	const std::unordered_map< std::string, Value > PREDEF_OPERATORS = {
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
		},
		{
			"undefined",
			VALUE( type::Undefined ),
		},
	};

	const std::unordered_map< std::string, program::operator_type_t > OPERATOR_NAMES = {
		{
			"return",
			program::OT_RETURN
		},
		{
			"throw",
			program::OT_THROW
		},
		{
			"=",
			program::OT_ASSIGN
		},
		{
			"!",
			program::OT_NOT
		},
		{
			"==",
			program::OT_EQ
		},
		{
			"!=",
			program::OT_NE
		},
		{
			"<",
			program::OT_LT
		},
		{
			"<=",
			program::OT_LTE
		},
		{
			">",
			program::OT_GT
		},
		{
			">=",
			program::OT_GTE
		},
		{
			"&&",
			program::OT_AND
		},
		{
			"||",
			program::OT_OR
		},
		{
			"+",
			program::OT_ADD
		},
		{
			"-",
			program::OT_SUB
		},
		{
			"*",
			program::OT_MULT
		},
		{
			"/",
			program::OT_DIV
		},
		{
			"%",
			program::OT_MOD
		},
		{
			"++",
			program::OT_INC
		},
		{
			"--",
			program::OT_DEC
		},
		{
			"+=",
			program::OT_INC_BY
		},
		{
			"-=",
			program::OT_DEC_BY
		},
		{
			"*=",
			program::OT_MULT_BY
		},
		{
			"/=",
			program::OT_DIV_BY
		},
		{
			"%/",
			program::OT_MOD_BY
		},
		{
			".",
			program::OT_CHILD
		},
		{
			"[]",
			program::OT_AT
		},
		{
			"[]=",
			program::OT_APPEND
		},
		{
			":",
			program::OT_RANGE
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
	const std::unordered_map< program::operator_type_t, const operator_info_t > OPERATOR_INFO = {
		{
			program::OT_RETURN,
			{
				1,
				OL_RIGHT
			}
		},
		{
			program::OT_THROW,
			{
				1,
				OL_RIGHT
			}
		},
		{
			program::OT_ASSIGN,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::OT_NOT,
			{
				14,
				OL_RIGHT
			}
		},
		{
			program::OT_EQ,
			{
				8,
				OL_BOTH
			}
		},
		{
			program::OT_NE,
			{
				8,
				OL_BOTH
			}
		},
		{
			program::OT_LT,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::OT_LTE,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::OT_GT,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::OT_GTE,
			{
				9,
				OL_BOTH
			}
		},
		{
			program::OT_AND,
			{
				4,
				OL_BOTH
			}
		},
		{
			program::OT_OR,
			{
				3,
				OL_BOTH
			}
		},
		{
			program::OT_ADD,
			{
				11,
				OL_BOTH
			}
		},
		{
			program::OT_SUB,
			{
				11,
				OL_BOTH
			}
		},
		{
			program::OT_MULT,
			{
				12,
				OL_BOTH
			}
		},
		{
			program::OT_DIV,
			{
				12,
				OL_BOTH
			}
		},
		{
			program::OT_MOD,
			{
				12,
				OL_BOTH
			}
		},
		{
			program::OT_INC,
			{
				15,
				OL_ANY
			}
		},
		{
			program::OT_DEC,
			{
				15,
				OL_ANY
			}
		},
		{
			program::OT_INC_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::OT_DEC_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::OT_MULT_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::OT_DIV_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::OT_MOD_BY,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::OT_CHILD,
			{
				17,
				OL_BOTH
			}
		},
		{
			program::OT_AT,
			{
				18,
				OL_BOTH
			}
		},
		{
			program::OT_APPEND,
			{
				2,
				OL_BOTH
			}
		},
		{
			program::OT_RANGE,
			{
				3,
				OL_ANY_OR_BOTH
			}
		}
	};

	const source_elements_t::const_iterator GetBracketsEnd( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const;
	const bool IsObject( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const;

#ifdef DEBUG
	void LogElement( const std::string& prefix, const SourceElement* element ) const;
	void LogElements( const std::string& label, const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const;
#endif

};

}
}
