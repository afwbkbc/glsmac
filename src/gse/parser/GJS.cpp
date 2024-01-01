#include "GJS.h"

#include <stack>
#include <functional>

#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"

#include "gse/program/Variable.h"
#include "gse/program/Value.h"
#include "gse/program/Array.h"
#include "gse/program/Object.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"
#include "gse/program/If.h"
#include "gse/program/ElseIf.h"
#include "gse/program/Else.h"
#include "gse/program/While.h"
#include "gse/program/Try.h"

namespace gse {
namespace parser {

using namespace program;

GJS::GJS( const std::string& filename, const std::string& source )
	: Parser( filename, source ) {

}

void GJS::GetElements( source_elements_t& elements ) {
	char c;
	si_t::pos_t begin;
	si_t si;
	std::string value;
	std::unordered_map< std::string, Parser::Conditional::conditional_type_t >::const_iterator control_it;
	while ( !eof() ) {
		begin = get_si_pos();
		if ( match_sequence( "//", true ) ) {
			value = read_until_char_any( CHARS_EOLN.c_str(), false );
			elements.push_back( new Comment( false, value, get_si( begin, get_si_pos() ) ) );
		}
		else if ( match_sequence( "/*", true ) ) {
			value = read_until_sequence( "*/", true );
			elements.push_back( new Comment( true, value, get_si( begin, get_si_pos() ) ) );
		}
		else if ( ( c = match_char_any( CHARS_QUOTES.c_str(), true ) ) ) {
			value = read_until_char( c, true );
			elements.push_back( new Identifier( value, IDENTIFIER_STRING, get_si( begin, get_si_pos() ) ) );
		}
		else if ( match_char_any( CHARS_WHITESPACE.c_str(), true ) ) {
			skip_while_char_any( CHARS_WHITESPACE.c_str() );
		}
		else if ( match_char_any( CHARS_NUMBERS.c_str(), false ) ) {
			value = read_while_char_any( CHARS_NUMBERS_C.c_str() );
			elements.push_back( new Identifier( value, IDENTIFIER_NUMBER, get_si( begin, get_si_pos() ) ) );
		}
		else if ( match_char_any( CHARS_NAMES.c_str(), false ) ) {
			value = read_while_char_any( CHARS_NAMES_C.c_str() );
			si = get_si( begin, get_si_pos() );
			control_it = CONTROL_KEYWORDS.find( value );
			if ( control_it != CONTROL_KEYWORDS.end() ) {
				elements.push_back( new Conditional( control_it->second, si ) );
			}
			else if ( KEYWORDS.find( value ) != KEYWORDS.end() ) {
				elements.push_back( new Operator( value, si ) );
			}
			else {
				elements.push_back( new Identifier( value, IDENTIFIER_VARIABLE, si ) );
			}
		}
		else if ( match_char_any( CHARS_OPERATORS.c_str(), false ) ) {
			value = read_while_char_any( CHARS_OPERATORS.c_str() );
			elements.push_back( new Operator( value, get_si( begin, get_si_pos() ) ) );
		}
		else if ( ( c = match_char_any( CHARS_DELIMITERS.c_str(), true ) ) ) {
			si = get_si( begin, get_si_pos() );
			switch ( c ) {
				case ';': {
					elements.push_back( new Delimiter( Delimiter::DT_CODE, si ) );
					break;
				}
				case ',': {
					elements.push_back( new Delimiter( Delimiter::DT_DATA, si ) );
					break;
				}
				case '(': {
					elements.push_back( new Block( BLOCK_ROUND_BRACKETS, Block::BS_BEGIN, si ) );
					break;
				}
				case ')': {
					elements.push_back( new Block( BLOCK_ROUND_BRACKETS, Block::BS_END, si ) );
					break;
				}
				case '[': {
					elements.push_back( new Block( BLOCK_SQUARE_BRACKETS, Block::BS_BEGIN, si ) );
					break;
				}
				case ']': {
					elements.push_back( new Block( BLOCK_SQUARE_BRACKETS, Block::BS_END, si ) );
					break;
				}
				case '{': {
					elements.push_back( new Block( BLOCK_CURLY_BRACKETS, Block::BS_BEGIN, si ) );
					break;
				}
				case '}': {
					elements.push_back( new Block( BLOCK_CURLY_BRACKETS, Block::BS_END, si ) );
					break;
				}
				default:
					ASSERT( false, (std::string)"unexpected delimiter: " + c );
			}
		}
		else {
			ASSERT( false, (std::string)"unexpected character: " + get() + " (" + std::to_string( get() ) + ")" );
		}
	}
}

#ifdef DEBUG
#define ELS( _label ) LogElements( _label, begin, end );
#define EL( _label ) LogElement( _label ": ", element );
#else
#define ELS( _label )
#define EL()
#endif

const program::Program* GJS::GetProgram( const source_elements_t& elements ) {
	// TODO

	NEWV( program, program::Program, GetScope( elements.begin(), elements.end() ) );
	return program;
}

const si_t GJS::GetSI( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	return {
		( *begin )->m_si.file,
		( *begin )->m_si.from,
		( ( *( end - 1 ) )->m_si.to )
	};
}

const program::Scope* GJS::GetScope( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetScope" )
	std::vector< const program::Control* > body = {};
	auto it = begin;
	source_elements_t::const_iterator it_end;
	std::stack< uint8_t > brackets = {};
	while ( it != end ) {
		it_end = it;
		while ( it_end != end ) {
			if ( ( *it_end )->m_type == SourceElement::ET_BLOCK ) {
				switch ( ( ( Block * )( *it_end ) )->m_block_side ) {
					case Block::BS_BEGIN: {
						brackets.push( ( ( Block * )( *it_end ) )->m_block_type );
						break;
					}
					case Block::BS_END: {
						ASSERT( !brackets.empty(), "block closed but not opened" );
						ASSERT( brackets.top() == ( ( Block * )( *it_end ) )->m_block_type, "block close of different type" );
						brackets.pop();
						break;
					}
					default: {
						ASSERT( false, "unexpected block side: " + std::to_string( ( ( Block * )( *it_end ) )->m_block_side ) );
					}
				}
			}
			else if (
				brackets.empty() &&
					( *it_end )->m_type == SourceElement::ET_DELIMITER &&
					( ( Delimiter * )( *it_end ) )->m_delimiter_type == Delimiter::DT_CODE
				) {
				if ( it != it_end ) {
					body.push_back( GetControl( it, it_end ) );
				}
				break;
			}
			if ( it_end != end ) {
				it_end++;
			}
		}
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Scope( GetSI( begin, end ), body );
}

const program::Control* GJS::GetControl( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetControl" );
	ASSERT( begin != end, "no elements inside" );
	if ( ( *begin )->m_type == SourceElement::ET_CONDITIONAL ) {
		return GetConditional( begin, end );
	}
	else {
		return GetStatement( begin, end );
	}
}

const program::Conditional* GJS::GetConditional( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetConditional" );
	ASSERT( ( *begin )->m_type == SourceElement::ET_CONDITIONAL, "conditional expected here" );
	Conditional* conditional = (Conditional*)( *begin );
	source_elements_t::const_iterator it = begin + 1, it_end;
	const program::Expression* condition = nullptr;
	if ( conditional->has_condition ) {
		ASSERT(
			it != end &&
				( *it )->m_type == SourceElement::ET_BLOCK &&
				( ( Block * )( *it ) )->m_block_side == Block::BS_BEGIN &&
				( ( Block * )( *it ) )->m_block_type == BLOCK_ROUND_BRACKETS, "expected ( here " );
		it_end = GetBracketsEnd( it, end );
		condition = GetExpression( it + 1, it_end );
		ASSERT( it_end != end, "expected { here" );
		it = it_end + 1;
	}
	ASSERT(
		it != end &&
			( *it )->m_type == SourceElement::ET_BLOCK &&
			( ( Block * )( *it ) )->m_block_side == Block::BS_BEGIN &&
			( ( Block * )( *it ) )->m_block_type == BLOCK_CURLY_BRACKETS, "expected { here" );
	it_end = GetBracketsEnd( it, end );
	const auto* body = GetScope( it + 1, it_end );

	it = it_end;
	if ( it != end ) {
		it++;
	}

	switch ( conditional->m_conditional_type ) {
		case Conditional::CT_IF:
		case Conditional::CT_ELSEIF: {
			const program::Conditional* els = nullptr;
			const auto si = GetSI( begin, it );
			if ( it != end ) {
				els = GetConditional( it, end );
				it++;
			}
			if ( conditional->m_conditional_type == Conditional::CT_IF ) {
				return new program::If( si, condition, body, els );
			}
			else {
				return new program::ElseIf( si, condition, body, els );
			}
		}
		case Conditional::CT_ELSE:
			return new program::Else( GetSI( begin, end ), body );
		case Conditional::CT_WHILE:
			return new program::While( GetSI( begin, end ), condition, body );
		case Conditional::CT_TRY: {
			ASSERT(
				it != end &&
					( *it )->m_type == SourceElement::ET_CONDITIONAL &&
					( (Conditional*)( *it ) )->m_conditional_type == Conditional::CT_CATCH &&
					it + 1 != end &&
					( *( it + 1 ) )->m_type == SourceElement::ET_BLOCK &&
					( ( Block * )( *( it + 1 ) ) )->m_block_side == Block::BS_BEGIN &&
					( ( Block * )( *( it + 1 ) ) )->m_block_type == BLOCK_CURLY_BRACKETS, "try block without catch block" );
			it_end = GetBracketsEnd( it + 1, end );
			return new program::Try( GetSI( begin, it ), body, new program::Catch( GetSI( it, it_end + 1 ), GetObject( it + 2, it_end ) ) );
		}
		default:
			ASSERT( false, "unexpected conditional type: " + conditional->ToString() );
	}
}

const program::Statement* GJS::GetStatement( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetStatement" );
	return new program::Statement( GetSI( begin, end ), GetExpression( begin, end, true ) );
}

const program::Operand* GJS::GetExpressionOrOperand( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end, bool is_scope_expected ) {
	ELS( "GetExpression" )
	// resolve elements, unpack scopes
	typedef std::vector< const Element* > elements_t;
	elements_t elements = {};
	source_elements_t::const_iterator it = begin, it_end, it_next, it_tmp;
	uint8_t t;
	bool var_hints_allowed = true;
	Variable::variable_hints_t next_var_hints = Variable::VH_NONE;
	const Expression* condition;
	while ( it != end ) {
		switch ( ( *it )->m_type ) {
			case SourceElement::ET_COMMENT: {
				// nothing
				break;
			}
			case SourceElement::ET_IDENTIFIER: {
				var_hints_allowed = false;
				is_scope_expected = false;
				// check for child dereferences
				it_tmp = it + 1;
				while ( it_tmp != end ) {
					if (
						( *it_tmp )->m_type != SourceElement::ET_OPERATOR ||
							( (Operator*)( *it_tmp ) )->m_op != "."
						) {
						break;
					}
					it_tmp++;
					ASSERT( it_tmp != end &&
						( *it_tmp )->m_type == SourceElement::ET_IDENTIFIER &&
						( ( Identifier * )( *it_tmp ) )->m_identifier_type == IDENTIFIER_VARIABLE, "expected property name" );
					it_tmp++;
				}
				if ( it_tmp == it + 1 || it_tmp == end ) {
					// no child dereferences, treat as single operand
					elements.push_back( GetOperand( ( Identifier * )( *it ), &next_var_hints ) );
				}
				else {
					// child dereference, group it into expression
					ASSERT( next_var_hints == Variable::VH_NONE, "variable modifier can't be used with properties" );
					elements.push_back( GetExpressionOrOperand( it, it_tmp ) );
					it = it_tmp - 1;
				}
				break;
			}
			case SourceElement::ET_OPERATOR: {
				const auto& op = ( (Operator*)( *it ) )->m_op;
				is_scope_expected = false;
				const auto mod_it = MODIFIER_OPERATORS.find( op );
				if ( mod_it != MODIFIER_OPERATORS.end() ) {
					ASSERT(
						it + 1 != end &&
							( *( it + 1 ) )->m_type == SourceElement::ET_IDENTIFIER &&
							( ( Identifier * ) * ( it + 1 ) )->m_identifier_type == IDENTIFIER_VARIABLE &&
							it + 2 != end &&
							( *( it + 2 ) )->m_type == SourceElement::ET_OPERATOR &&
							( (Operator*)*( it + 2 ) )->m_op == "=" &&
							it + 3 != end, "expected variable and assignment operator after assignment modifier" );
					ASSERT( next_var_hints == Variable::VH_NONE, "multiple variable hints" );
					ASSERT( var_hints_allowed, "variable hints are not allowed here" );
					next_var_hints = mod_it->second;
					break;
				}
				const auto predef_it = PREDEF_OPERATORS.find( op );
				if ( predef_it != PREDEF_OPERATORS.end() ) {
					elements.push_back( new program::Value( ( *it )->m_si, predef_it->second ) );
				}
				else {
					elements.push_back( GetOperator( (Operator*)( *it ) ) );
				}
				var_hints_allowed = false;
				break;
			}
			case SourceElement::ET_CONDITIONAL: {
				ASSERT( false, "unexpected control" );
				break;
			}
			case SourceElement::ET_BLOCK: {
				var_hints_allowed = false;
				ASSERT( ( ( Block * )( *it ) )->m_block_side == Block::BS_BEGIN, "unexpected block side: " + std::to_string( ( ( Block * )( *it ) )->m_block_side ) );
				t = ( ( Block * )( *it ) )->m_block_type;
				it_end = GetBracketsEnd( it, end );
				switch ( t ) {
					case BLOCK_CURLY_BRACKETS: {
						// either scope or object
						if ( is_scope_expected ) {
							// scope
							elements.push_back( GetScope( it + 1, it_end ) );
						}
						else {
							// object
							elements.push_back( GetObject( it + 1, it_end ) );
						}
						break;
					}
					case BLOCK_ROUND_BRACKETS: {
						// expression, function or call
						if (
							(
								( it == begin ) ||
									( ( *( it - 1 ) )->m_type == SourceElement::ET_OPERATOR )
							) &&
								(
									( it_end + 1 == end ) ||
										( ( *( it_end + 1 ) )->m_type != SourceElement::ET_OPERATOR ) ||
										( (Operator*)*( it_end + 1 ) )->m_op != "=>"
								)
							) {
							// expression
							elements.push_back( GetExpression( it + 1, it_end ) );
						}
						else if (
							it_end != end &&
								( it_next = it_end + 1 ) != end &&
								( *it_next )->m_type == SourceElement::ET_OPERATOR &&
								( (Operator*)*( it_next++ ) )->m_op == "=>" &&
								( it_next != end ) &&
								( *it_next )->m_type == SourceElement::ET_BLOCK &&
								( ( Block * ) * it_next )->m_block_type == BLOCK_CURLY_BRACKETS &&
								( ( Block * ) * it_next )->m_block_side == Block::BS_BEGIN
							) {
							// function
							std::vector< Variable* > parameters = {};
							if ( it_end != it + 1 ) {
								bool expect_var = true;
								for ( it_tmp = it + 1 ; it_tmp != it_end ; it_tmp++ ) {
									if ( expect_var ) {
										if (
											( *it_tmp )->m_type == SourceElement::ET_IDENTIFIER &&
												( ( Identifier * )( *it_tmp ) )->m_identifier_type == IDENTIFIER_VARIABLE
											) {
											parameters.push_back( new Variable( ( *it_tmp )->m_si, ( ( Identifier * )( *it_tmp ) )->m_name ) );
										}
										else {
											ASSERT( false, "expected variable, got something else" );
										}
									}
									else {
										if (
											( *it_tmp )->m_type == SourceElement::ET_DELIMITER &&
												( ( Delimiter * ) * it_tmp )->m_delimiter_type == Delimiter::DT_DATA
											) {
											// nothing
										}
										else {
											ASSERT( false, "expected comma, got something else" );
										}
									}
									expect_var = !expect_var;
								}
								ASSERT( !expect_var, "expected variable, got nothing" );
							}
							it_end = GetBracketsEnd( it_next, end );
							elements.push_back( new program::Function( GetSI( it, it_end + 1 ), parameters, GetScope( it_next + 1, it_end ) ) );
						}
						else {
							// call
							ASSERT( !elements.empty(), "expected callable, found nothing" );

							auto* element = elements.back(); // use last element as callable
							ASSERT( element->m_element_type == Element::ET_OPERAND, "callable is not operand" );
							auto* operand = (Operand*)element;
							elements.pop_back();

							std::vector< const Expression* > arguments = {};
							it_end = GetBracketsEnd( it, end );
							it_next = it + 1;
							while ( it_next != it_end ) {
								it_tmp = it_next;
								while ( it_tmp != it_end ) {
									if ( ( *it_tmp )->m_type == SourceElement::ET_BLOCK ) {
										it_tmp = GetBracketsEnd( it_tmp, it_end );
										if ( it_tmp != it_end ) {
											it_tmp++;
										}
									}
									if (
										( *it_tmp )->m_type == SourceElement::ET_DELIMITER &&
											( ( Delimiter * )( *it_tmp ) )->m_delimiter_type == Delimiter::DT_DATA
										) {

										break;
									}
									if ( it_tmp != it_end ) {
										it_tmp++;
									}
								}
								arguments.push_back( GetExpression( it_next, it_tmp ) );
								it_next = it_tmp;
								if ( it_next != it_end ) {
									it_next++;
								}
							}
							elements.push_back(
								new Call(
									{
										element->m_si.file,
										element->m_si.from,
										( *it_end )->m_si.to
									},
									operand->type == Operand::OT_EXPRESSION
										? (Expression*)operand
										: new Expression( operand->m_si, operand ), arguments
								)
							);
						}
						break;
					}
					case BLOCK_SQUARE_BRACKETS: {
						// either array operator or array definition
						ASSERT( it != begin, "array syntax unexpected here" );
						switch ( ( *( it - 1 ) )->m_type ) {
							case SourceElement::ET_IDENTIFIER: {
								// array operator
								if (
									it_end != end &&
										it + 1 == it_end &&
										it_end + 1 != end &&
										( *( it_end + 1 ) )->m_type == SourceElement::ET_OPERATOR &&
										( (Operator*)( *( it_end + 1 ) ) )->m_op == "="
									) {
									// 'append' operator ( []= )
									ASSERT( it + 2 != end, "value expected after append operator" );
									elements.push_back( new program::Operator( GetSI( it, it_end + 2 ), program::Operator::OT_APPEND ) );
									it_end++;
								}
								else {
									// 'at' operator ( [i] )
									elements.push_back( new program::Operator( GetSI( it, it_end + 1 ), program::Operator::OT_AT ) );
									elements.push_back( GetExpressionOrOperand( it + 1, it_end ) );
								}
								break;
							}
							case SourceElement::ET_OPERATOR: {
								// array definition
								elements.push_back( GetArray( it + 1, it_end ) );
								break;
							}
							default: {
								ASSERT( false, "unexpected element type" );
							}
						}
						break;
					}
					default: {
						ASSERT( false, "unexpected block type: " + std::to_string( t ) );
					}
				}
				it = it_end;
				break;
			}
			default: {
				ASSERT( false, "unexpected expression element type: " + std::to_string( ( *it )->m_type ) );
			}
		}
		if ( !var_hints_allowed ) {
			ASSERT( next_var_hints == Variable::VH_NONE, "variable name required after modifier" );
		}
		if ( it != end ) {
			it++;
		}
	}

	const auto get_si = []( const elements_t::const_iterator& begin, const elements_t::const_iterator& end ) -> si_t {
		return {
			( *begin )->m_si.file,
			( *begin )->m_si.from,
			( *( end - 1 ) )->m_si.to
		};
	};

	// split by operator priority
	const std::function<
		program::Operand*(
			const elements_t::const_iterator&,
			const elements_t::const_iterator&
		)
	> get_operand = [
		this,
		&get_operand,
		&get_si
	](
		const elements_t::const_iterator& begin,
		const elements_t::const_iterator& end
	) -> program::Operand* {
		if ( begin + 1 == end ) {
			// only one operand present, wrap if needed and return
			ASSERT( ( *begin )->m_element_type == Element::ET_OPERAND, "operand expected here" );
			return (program::Operand*)*begin;
		}
		// find most important operator
		const program::Operator* op;
		uint8_t last_priority = 255;
		elements_t::const_iterator split_it = end;
		const operator_info_t* info;
		operator_link_t link;
		const Element* el;
		for ( auto it = begin ; it != end ; it++ ) {
			el = *it;
			if ( el->m_element_type == Element::ET_OPERATOR ) {
				op = (program::Operator*)( *it );
				info = &OPERATOR_INFO.at( op->op );
				if ( info->priority <= last_priority ) {
					last_priority = info->priority;
					split_it = it;
					link = info->link;
				}
			}
		}
		ASSERT( split_it != end, "could not find operator to split at" );

		bool has_a = split_it > begin;
		bool has_b = split_it + 1 != end;

		si_t si = ( *split_it )->m_si;

		for ( auto it = begin ; it != end ; it++ ) {
			el = *it;
			if (
				it < split_it &&
					(
						el->m_si.from.line < si.from.line ||
							(
								el->m_si.from.line == si.from.line &&
									el->m_si.from.col < si.from.col
							)
					)
				) {
				si.from = el->m_si.from;
			}
			if (
				it > split_it &&
					( el->m_si.to.line > si.to.line ||
						(
							el->m_si.to.line == si.to.line &&
								el->m_si.to.col > si.to.col
						)
					) ) {
				si.to = el->m_si.to;
			}
		}
		switch ( link ) {
			case OL_LEFT: {
				ASSERT( has_a, "left operand is missing" );
				return new program::Expression( si, get_operand( begin, split_it ), (program::Operator*)*split_it );
			}
			case OL_RIGHT: {
				ASSERT( has_b, "right operand is missing" );
				return new program::Expression( si, nullptr, (program::Operator*)*split_it, get_operand( split_it + 1, end ) );
			}
			case OL_BOTH: {
				ASSERT( has_a, "left operand is missing" );
				ASSERT( has_b, "right operand is missing" );
				return new program::Expression( si, get_operand( begin, split_it ), (program::Operator*)*split_it, get_operand( split_it + 1, end ) );
			}
			case OL_ANY: {
				ASSERT( !has_a || !has_b, "both left and right operands are present" );
				case OL_ANY_OR_BOTH:
					ASSERT( has_a || has_b, "both left and right operands are missing" );
				return new program::Expression(
					si,
					has_a
						? get_operand( begin, split_it )
						: nullptr,
					(program::Operator*)*split_it,
					has_b
						? get_operand( split_it + 1, end )
						: nullptr
				);
			}
			default:
				ASSERT( false, "unexpected operator link type: " + std::to_string( link ) );
		}
	};

	return get_operand( elements.begin(), elements.end() );
}

const program::Expression* GJS::GetExpression( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end, const bool is_scope_expected ) {
	const auto* operand = GetExpressionOrOperand( begin, end, is_scope_expected );
	return operand->type == program::Operand::OT_EXPRESSION
		? (Expression*)operand
		: new Expression( operand->m_si, operand );
}

const program::Operand* GJS::GetOperand( const Identifier* element, program::Variable::variable_hints_t* next_var_hints ) {
	EL( "GetOperand" )
	switch ( element->m_identifier_type ) {
		case IDENTIFIER_VARIABLE: {
			program::Variable::variable_hints_t hints = Variable::VH_NONE;
			if ( next_var_hints ) {
				hints = *next_var_hints;
				*next_var_hints = Variable::VH_NONE;
			}
			return new Variable( element->m_si, element->m_name, hints );
		}
		case IDENTIFIER_NUMBER: {
			try {
				// maybe it's int?
				const auto v = std::stoi( element->m_name.c_str() );
				return new program::Value( element->m_si, VALUE( type::Int, v ) );
			}
			catch ( std::logic_error const& ex ) {
				try {
					// maybe it's float?
					const auto f = std::stof( element->m_name.c_str() );
					return new program::Value( element->m_si, VALUE( type::Float, f ) );
				}
				catch ( std::logic_error const& ex ) {
					ASSERT( false, "value is not a number: " + element->m_name );
				}
			}
		}
		case IDENTIFIER_STRING: {
			return new program::Value( element->m_si, VALUE( type::String, element->m_name ) );
		}
		default:
			ASSERT( false, "unexpected identifier type: " + std::to_string( element->m_identifier_type ) );
	}
}

const program::Operator* GJS::GetOperator( const Operator* element ) {
	EL( "GetOperator" )
	const auto it = OPERATOR_NAMES.find( element->m_op );
	ASSERT( it != OPERATOR_NAMES.end(), "operator name not found: " + element->m_op );
	return new program::Operator( element->m_si, it->second );
}

const program::Array* GJS::GetArray( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetArray" );
	std::vector< const Expression* > elements = {};
	source_elements_t::const_iterator it = begin, it_end;
	while ( it != end ) {
		// find element expression end
		for ( it_end = it ; it_end != end ; it_end++ ) {
			if ( ( *it_end )->m_type == SourceElement::ET_BLOCK ) {
				it_end = GetBracketsEnd( it_end, end );
			}
			else if (
				( *it_end )->m_type == SourceElement::ET_DELIMITER ) {
				ASSERT( ( ( Delimiter * ) * it_end )->m_delimiter_type == Delimiter::DT_DATA, "unexpected delimiter type: " + ( *it_end )->ToString() );
				break;
			}
		}
		ASSERT( it_end != it, "expected element expression" );
		elements.push_back( GetExpression( it, it_end ) );
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Array( GetSI( begin - 1, end + 1 ), elements );
}

const program::Object* GJS::GetObject( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetObject" );
	std::unordered_map< std::string, const Expression* > properties = {};
	Identifier* identifier;
	source_elements_t::const_iterator it = begin, it_end;
	while ( it != end ) {
		ASSERT( ( *it )->m_type == SourceElement::ET_IDENTIFIER, "expected property key" );
		identifier = ( Identifier * ) * ( it++ );
		ASSERT( it != end, "unexpected object close" );
		ASSERT( identifier->m_identifier_type == IDENTIFIER_VARIABLE, "expected property key name" );
		ASSERT( ( *it )->m_type == SourceElement::ET_OPERATOR, "expected property delimiter" );
		ASSERT( ( (Operator*)( *( it++ ) ) )->m_op == ":", "expected ':'" );
		ASSERT( it != end, "unexpected object close" );

		// find property expression end
		for ( it_end = it ; it_end != end ; it_end++ ) {
			if ( ( *it_end )->m_type == SourceElement::ET_BLOCK ) {
				it_end = GetBracketsEnd( it_end, end );
			}
			else if (
				( *it_end )->m_type == SourceElement::ET_DELIMITER ) {
				ASSERT( ( ( Delimiter * ) * it_end )->m_delimiter_type == Delimiter::DT_DATA, "unexpected delimiter type: " + ( *it_end )->ToString() );
				break;
			}
		}
		ASSERT( it_end != it, "expected property expression" );
		ASSERT( properties.find( identifier->m_name ) == properties.end(), "duplicate property key: " + identifier->m_name );
		properties.insert_or_assign( identifier->m_name, GetExpression( it, it_end ) );
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Object( GetSI( begin - 1, end + 1 ), properties );
}

const GJS::source_elements_t::const_iterator GJS::GetBracketsEnd( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ASSERT( ( *begin )->m_type == SourceElement::ET_BLOCK, "expected opening bracket" );
	std::stack< uint8_t > brackets = {};
	source_elements_t::const_iterator it = begin;
	Block* block = ( Block * ) * begin;
	ASSERT( block->m_block_side == Block::BS_BEGIN, "expected opening bracket" );
	while ( it != end ) {
		if ( ( *it )->m_type == SourceElement::ET_BLOCK ) {
			block = ( Block * )( *it );
			switch ( block->m_block_side ) {
				case Block::BS_BEGIN: {
					brackets.push( block->m_block_type );
					break;
				}
				case Block::BS_END: {
					ASSERT( !brackets.empty(), "block closed but not opened" );
					ASSERT( brackets.top() == block->m_block_type, "block close of different type" );
					brackets.pop();
					break;
				}
			}
			if ( brackets.empty() ) {
				break;
			}
		}
		if ( it != end ) {
			it++;
		}
	}
	ASSERT( brackets.empty(), "matching closing block not found" );
	return it;
}

#ifdef DEBUG

void GJS::LogElement( const std::string& prefix, const SourceElement* element ) const {
	Log( prefix + element->ToString() );
}

void GJS::LogElements( const std::string& label, const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const {
	Log( label + "(" );
	for ( auto it = begin ; it != end ; it++ ) {
		LogElement( "    ", *it );
	}
	Log( ")" );
}

#endif

#undef ELS
#undef EL

}
}
