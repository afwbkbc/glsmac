#include "JS.h"

#include <stack>
#include <functional>

#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"

#include "gse/Exception.h"

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

JS::JS( const std::string& filename, const std::string& source, const size_t initial_line_num )
	: Parser( filename, source, initial_line_num ) {

}

void JS::GetElements( source_elements_t& elements ) {
	char c;
	si_t::pos_t begin;
	si_t si;
	std::string value;
	std::unordered_map< std::string, Parser::Conditional::conditional_type_t >::const_iterator control_it;
	while ( !eof() ) {
		begin = get_si_pos();
		if ( match_sequence( "//", true ) ) {
			skip_until_char_any( CHARS_EOLN.c_str(), true );
		}
		else if ( match_sequence( "/*", true ) ) {
			skip_until_sequence( "*/", true );
		}
		else if ( ( c = match_char_any( CHARS_QUOTES.c_str(), true ) ) ) {
			value = unpack_backslashes( read_until_char( c, true, true ) );
			elements.push_back( new Identifier( value, IDENTIFIER_STRING, make_si( begin, get_si_pos() ) ) );
		}
		else if ( match_char_any( CHARS_WHITESPACE.c_str(), true ) ) {
			skip_while_char_any( CHARS_WHITESPACE.c_str() );
		}
		else if ( match_char_any( CHARS_NUMBERS.c_str(), false ) ) {
			value = read_while_char_any( CHARS_NUMBERS_C.c_str() );
			elements.push_back( new Identifier( value, IDENTIFIER_NUMBER, make_si( begin, get_si_pos() ) ) );
		}
		else if ( ( c = match_char_any( CHARS_NAMES.c_str(), true ) ) ) {
			value = c + read_while_char_any( CHARS_NAMES_C.c_str() );
			si = make_si( begin, get_si_pos() );
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
			si = make_si( begin, get_si_pos() );
			if (
				( value == "-" || value == "+" ) &&
					check_char_any( CHARS_NUMBERS.c_str() ) &&
					( elements.empty() ||
						elements.back()->m_type == SourceElement::ET_OPERATOR ||
						(
							elements.back()->m_type == SourceElement::ET_BLOCK &&
								( (Block*)elements.back() )->m_block_side == Block::BS_BEGIN
						)
					)
				) {
				// negative number
				value += read_while_char_any( CHARS_NUMBERS_C.c_str() );
				elements.push_back( new Identifier( value, IDENTIFIER_NUMBER, si ) );
			}
			else {
				elements.push_back( new Operator( value, si ) );
			}
		}
		else if ( ( c = match_char_any( CHARS_DELIMITERS.c_str(), true ) ) ) {
			si = make_si( begin, get_si_pos() );
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
					elements.push_back( new Block( block_info.BLOCK_ROUND_BRACKETS, Block::BS_BEGIN, si ) );
					break;
				}
				case ')': {
					elements.push_back( new Block( block_info.BLOCK_ROUND_BRACKETS, Block::BS_END, si ) );
					break;
				}
				case '[': {
					elements.push_back( new Block( block_info.BLOCK_SQUARE_BRACKETS, Block::BS_BEGIN, si ) );
					break;
				}
				case ']': {
					elements.push_back( new Block( block_info.BLOCK_SQUARE_BRACKETS, Block::BS_END, si ) );
					break;
				}
				case '{': {
					elements.push_back( new Block( block_info.BLOCK_CURLY_BRACKETS, Block::BS_BEGIN, si ) );
					break;
				}
				case '}': {
					elements.push_back( new Block( block_info.BLOCK_CURLY_BRACKETS, Block::BS_END, si ) );
					break;
				}
				default:
					THROW( (std::string)"Unexpected delimiter: '" + c + "'" );
			}
		}
		else {
			throw gse::Exception( EC.PARSE_ERROR, (std::string)"Unexpected character: " + get(), nullptr, make_si( begin, get_si_pos() ) );
		}
	}
}

#ifdef DEBUG
#define ELS( _label ) LogElements( _label, begin, end );
#define EL( _label ) LogElement( _label ": ", element );
#else
#define ELS( _label )
#define EL( _label )
#endif

const program::Program* JS::GetProgram( const source_elements_t& elements ) {
	NEWV( program, program::Program, GetScope( elements.begin(), elements.end() ) );
	return program;
}

const si_t JS::GetSI( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	if ( begin == end ) {
		return {};
	}
	else {
		return {
			( *begin )->m_si.file,
			( *begin )->m_si.from,
			( ( *( end - 1 ) )->m_si.to )
		};
	}
}

const program::Scope* JS::GetScope( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
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
						if ( brackets.empty() || brackets.top() != ( ( Block * )( *it_end ) )->m_block_type ) {
							throw gse::Exception( EC.PARSE_ERROR, "Unexpected block close: " + ( *it_end )->ToString(), nullptr, ( *it_end )->m_si );
						}
						brackets.pop();
						break;
					}
					default: {
						THROW( "unexpected block side: " + std::to_string( ( ( Block * )( *it_end ) )->m_block_side ) );
					}
				}
			}
			if ( brackets.empty() ) {
				if (
					it_end != end &&
						(
							(
								( *it )->m_type == SourceElement::ET_BLOCK &&
									( ( Block * )( *it ) )->m_block_type == BLOCK_CURLY_BRACKETS &&
									( ( Block * )( *it ) )->m_block_side == Block::BS_BEGIN
							) || (
								( *it )->m_type == SourceElement::ET_CONDITIONAL &&
									it_end + 1 != end &&
									(
										( *( it_end + 1 ) )->m_type != SourceElement::ET_CONDITIONAL || (
											( (Conditional*)*(it_end + 1))->m_conditional_type != Conditional::CT_ELSE &&
											( (Conditional*)*(it_end + 1))->m_conditional_type != Conditional::CT_ELSEIF &&
											( (Conditional*)*(it_end + 1))->m_conditional_type != Conditional::CT_CATCH
										)
									)
							)
						)
						&&
							( *it_end )->m_type == SourceElement::ET_BLOCK &&
						( ( Block * )( *it_end ) )->m_block_type == BLOCK_CURLY_BRACKETS &&
						( ( Block * )( *it_end ) )->m_block_side == Block::BS_END &&
						!IsObject( it + 1, it_end )
					) {
					body.push_back( GetControl( it, it_end + 1 ) );
					break;
				}
				else if (
					( *it_end )->m_type == SourceElement::ET_DELIMITER &&
						( ( Delimiter * )( *it_end ) )->m_delimiter_type == Delimiter::DT_CODE
					) {
					if ( it != it_end ) {
						body.push_back( GetControl( it, it_end ) );
					}
					break;
				}
			}
			if ( it_end != end ) {
				it_end++;
			}
		}
		if ( it_end == end ) {
			body.push_back( GetControl( it, it_end ) );
		}
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Scope( GetSI( begin, end ), body );
}

const program::Control* JS::GetControl( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetControl" );
	ASSERT( begin != end, "no elements inside" );
	if ( ( *begin )->m_type == SourceElement::ET_CONDITIONAL ) {
		return GetConditional( begin, end );
	}
	else {
		return GetStatement( begin, end );
	}
}

const program::Conditional* JS::GetConditional( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetConditional" );
	ASSERT( ( *begin )->m_type == SourceElement::ET_CONDITIONAL, "conditional expected here" );
	Conditional* conditional = (Conditional*)( *begin );
	source_elements_t::const_iterator it = begin + 1, it_end;
	const program::Expression* condition = nullptr;
	if ( conditional->has_condition ) {
		if (
			it == end ||
				( *it )->m_type != SourceElement::ET_BLOCK ||
				( ( Block * )( *it ) )->m_block_side != Block::BS_BEGIN ||
				( ( Block * )( *it ) )->m_block_type != BLOCK_ROUND_BRACKETS ) {
			throw gse::Exception(
				EC.PARSE_ERROR, "Expected (" + ( it == end
					? " here"
					: ", got: " + ( *it )->ToString()
				), nullptr, ( *begin )->m_si
			);
		}
		it_end = GetBracketsEnd( it, end );
		condition = GetExpression( it + 1, it_end );
		ASSERT( it_end != end, "expected {, got EOF" );
		it = it_end + 1;
	}
	if (
		it == end ||
			( *it )->m_type != SourceElement::ET_BLOCK ||
			( ( Block * )( *it ) )->m_block_side != Block::BS_BEGIN ||
			( ( Block * )( *it ) )->m_block_type != BLOCK_CURLY_BRACKETS ) {
		throw gse::Exception(
			EC.PARSE_ERROR, "Expected {" + ( it == end
				? " here"
				: ", got: " + ( *it )->ToString()
			), nullptr, ( *begin )->m_si
		);
	}
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
			if (
				it == end ||
					( *it )->m_type != SourceElement::ET_CONDITIONAL ||
					( (Conditional*)( *it ) )->m_conditional_type != Conditional::CT_CATCH ||
					it + 1 == end ||
					( *( it + 1 ) )->m_type != SourceElement::ET_BLOCK ||
					( ( Block * )( *( it + 1 ) ) )->m_block_side != Block::BS_BEGIN ||
					( ( Block * )( *( it + 1 ) ) )->m_block_type != BLOCK_CURLY_BRACKETS ) {
				throw gse::Exception(
					EC.PARSE_ERROR, "Expected catch block" + ( it == end
						? " here"
						: ", got: " + ( *it )->ToString()
					), nullptr, ( *( it - 1 ) )->m_si
				);
			}
			it_end = GetBracketsEnd( it + 1, end );
			return new program::Try( GetSI( begin, it ), body, new program::Catch( GetSI( it, it_end + 1 ), GetObject( it + 2, it_end ) ) );
		}
		case Conditional::CT_CATCH: {
			throw gse::Exception( EC.PARSE_ERROR, "Unexpected catch without try", nullptr, conditional->m_si );
		}
		default:
			THROW( "unexpected conditional type: " + conditional->ToString() );
	}
}

const program::Statement* JS::GetStatement( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetStatement" );
	return new program::Statement( GetSI( begin, end ), GetExpression( begin, end ) );
}

const program::Operand* JS::GetExpressionOrOperand( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
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
					if ( it_tmp == end ||
						( *it_tmp )->m_type != SourceElement::ET_IDENTIFIER ||
						( ( Identifier * )( *it_tmp ) )->m_identifier_type != IDENTIFIER_VARIABLE ) {
						throw gse::Exception(
							EC.PARSE_ERROR, "Expected property name" + ( it_tmp == end
								? " here"
								: ", got: " + ( *( it_tmp ) )->ToString()
							), nullptr, ( *it )->m_si
						);
					}
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
				const auto mod_it = MODIFIER_OPERATORS.find( op );
				if ( mod_it != MODIFIER_OPERATORS.end() ) {
					if ( it + 1 == end ||
						( *( it + 1 ) )->m_type != SourceElement::ET_IDENTIFIER ||
						( ( Identifier * ) * ( it + 1 ) )->m_identifier_type != IDENTIFIER_VARIABLE
						) {
						throw gse::Exception(
							EC.PARSE_ERROR, "Expected variable name" + ( it + 1 == end
								? " after " + mod_it->first
								: ", got: " + ( *( it + 1 ) )->ToString()
							), nullptr, ( *it )->m_si
						);
					}
					if ( it + 2 == end ||
						( *( it + 2 ) )->m_type != SourceElement::ET_OPERATOR ||
						( (Operator*)*( it + 2 ) )->m_op != "="
						) {
						throw gse::Exception(
							EC.PARSE_ERROR, "Expected =" + ( it + 2 == end
								? " after variable name"
								: ", got: " + ( *( it + 2 ) )->ToString()
							), nullptr, ( *( it + 1 ) )->m_si
						);
					}
					if ( it + 3 == end ) {
						throw gse::Exception( EC.PARSE_ERROR, "Expected value after =", nullptr, ( *( it + 2 ) )->m_si );
					}
					ASSERT( next_var_hints == Variable::VH_NONE, "multiple variable hints" );
					if ( !var_hints_allowed ) {
						throw gse::Exception( EC.PARSE_ERROR, "Unexpected keyword: " + mod_it->first, nullptr, ( *it )->m_si );
					}
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
				throw gse::Exception( EC.PARSE_ERROR, "Unexpected keyword: " + ( *it )->ToString() + "", nullptr, ( *it )->m_si );
			}
			case SourceElement::ET_BLOCK: {
				var_hints_allowed = false;
				ASSERT( ( ( Block * )( *it ) )->m_block_side == Block::BS_BEGIN, "unexpected block side: " + std::to_string( ( ( Block * )( *it ) )->m_block_side ) );
				t = ( ( Block * )( *it ) )->m_block_type;
				it_end = GetBracketsEnd( it, end );
				switch ( t ) {
					case BLOCK_CURLY_BRACKETS: {
						// either object or scope
						if ( IsObject( it + 1, it_end ) ) {
							// object
							elements.push_back( GetObject( it + 1, it_end ) );
						}
						else {
							// scope
							elements.push_back( GetScope( it + 1, it_end ) );
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
							if ( it + 1 == it_end ) {
								throw gse::Exception( EC.PARSE_ERROR, "Expected expression, got: " + ( *it_end )->ToString(), nullptr, ( *it_end )->m_si );
							}
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
											throw gse::Exception( EC.PARSE_ERROR, "Expected argument, found: " + ( *it_tmp )->ToString() + "", nullptr, ( *it_tmp )->m_si );
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
											throw gse::Exception( EC.PARSE_ERROR, "Expected argument separator, found: " + ( *it_tmp )->ToString() + "", nullptr, ( *it_tmp )->m_si );
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
							if ( elements.empty() ) {
								throw gse::Exception( EC.PARSE_ERROR, "Expected { after =>", nullptr, ( *it )->m_si );
							}

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
									}
									if ( it_tmp != it_end ) {
										it_tmp++;
									}
									if (
										( *it_tmp )->m_type == SourceElement::ET_DELIMITER &&
											( ( Delimiter * )( *it_tmp ) )->m_delimiter_type == Delimiter::DT_DATA
										) {
										break;
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
						if ( it == begin ) {
							// array definition (because it starts at beginning)
							elements.push_back( GetArray( it + 1, it_end ) );
						}
						else {
							switch ( ( *( it - 1 ) )->m_type ) {
								case SourceElement::ET_OPERATOR: {
									// array definition
									elements.push_back( GetArray( it + 1, it_end ) );
									break;
								}
								default: {
									//case SourceElement::ET_IDENTIFIER: {
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
									/*default: {
										THROW( "unexpected element type before array: " + (*( it - 1 ))->ToString() );
									}*/
							}
						}
						break;
					}
					default: {
						THROW( "unexpected block type: " + std::to_string( t ) );
					}
				}
				it = it_end;
				break;
			}
			default:
				throw gse::Exception( EC.PARSE_ERROR, "Unexpected: " + ( *it )->ToString() + "", nullptr, ( *it )->m_si );
		}
		if ( !var_hints_allowed ) {
			ASSERT( next_var_hints == Variable::VH_NONE, "variable name required after modifier" );
		}
		if ( it != end ) {
			it++;
		}
	}

	// split by operator priority
	const std::function<
		program::Operand*(
			const elements_t::const_iterator&,
			const elements_t::const_iterator&
		)
	> get_operand = [
		this,
		&get_operand
	](
		const elements_t::const_iterator& begin,
		const elements_t::const_iterator& end
	) -> program::Operand* {
		if ( begin + 1 == end ) {
			// only one operand present, wrap if needed and return
			if ( ( *begin )->m_element_type != Element::ET_OPERAND ) {
				throw gse::Exception( EC.PARSE_ERROR, "Unexpected: " + ( *begin )->ToString(), nullptr, ( *begin )->m_si );
			}
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
		if ( split_it == end ) {
			throw gse::Exception( EC.PARSE_ERROR, "Could not parse expression (forgot ; or operator?)", nullptr, ( *begin )->m_si );
		}

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
				if ( !has_a ) {
					throw gse::Exception( EC.PARSE_ERROR, "Left operand missing for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
				}
				if ( has_b ) {
					throw gse::Exception( EC.PARSE_ERROR, "Right operand unexpected for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
				}
				return new program::Expression( si, get_operand( begin, split_it ), (program::Operator*)*split_it );
			}
			case OL_RIGHT: {
				if ( !has_b ) {
					throw gse::Exception( EC.PARSE_ERROR, "Right operand missing for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
				}
				if ( has_a ) {
					throw gse::Exception( EC.PARSE_ERROR, "Left operand unexpected for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
				}
				return new program::Expression( si, nullptr, (program::Operator*)*split_it, get_operand( split_it + 1, end ) );
			}
			case OL_BOTH: {
				if ( !has_a ) {
					throw gse::Exception( EC.PARSE_ERROR, "Left operand missing for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
				}
				if ( !has_b ) {
					throw gse::Exception( EC.PARSE_ERROR, "Right operand missing for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
				}
				return new program::Expression( si, get_operand( begin, split_it ), (program::Operator*)*split_it, get_operand( split_it + 1, end ) );
			}
			case OL_ANY: {
				if ( has_a && has_b ) {
					throw gse::Exception( EC.PARSE_ERROR, "Either left or right operand is expected, both found, for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
				}
				case OL_ANY_OR_BOTH:
					if ( !has_a && !has_b ) {
						throw gse::Exception( EC.PARSE_ERROR, "Neither left nor right operand is found, for operator: " + ( *split_it )->ToString(), nullptr, ( *split_it )->m_si );
					}
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
				THROW( "unexpected operator link type: " + std::to_string( link ) );
		}
	};

	return get_operand( elements.begin(), elements.end() );
}

const program::Expression* JS::GetExpression( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	const auto* operand = GetExpressionOrOperand( begin, end );
	return operand->type == program::Operand::OT_EXPRESSION
		? (Expression*)operand
		: new Expression( operand->m_si, operand );
}

const program::Operand* JS::GetOperand( const Identifier* element, program::Variable::variable_hints_t* next_var_hints ) {
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
				const auto v = std::stol( element->m_name.c_str() );
				return new program::Value( element->m_si, VALUE( type::Int, v ) );
			}
			catch ( std::logic_error const& ex ) {
				try {
					// maybe it's float?
					const auto f = std::stof( element->m_name.c_str() );
					return new program::Value( element->m_si, VALUE( type::Float, f ) );
				}
				catch ( std::logic_error const& ex ) {
					THROW( "value is not a number: " + element->m_name );
				}
			}
		}
		case IDENTIFIER_STRING: {
			return new program::Value( element->m_si, VALUE( type::String, element->m_name ) );
		}
		default:
			THROW( "unexpected identifier type: " + std::to_string( element->m_identifier_type ) );
	}
}

const program::Operator* JS::GetOperator( const Operator* element ) {
	EL( "GetOperator" )
	const auto it = OPERATOR_NAMES.find( element->m_op );
	ASSERT( it != OPERATOR_NAMES.end(), "operator name not found: " + element->m_op );
	return new program::Operator( element->m_si, it->second );
}

const program::Array* JS::GetArray( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
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
				if ( ( ( Delimiter * ) * it_end )->m_delimiter_type != Delimiter::DT_DATA ) {
					throw gse::Exception( EC.PARSE_ERROR, "Expected: , got: " + ( *it_end )->ToString(), nullptr, ( *it_end )->m_si );
				}
				break;
			}
		}
		if ( it_end == it ) {
			throw gse::Exception( EC.PARSE_ERROR, "Expected array element, got: " + ( *it_end )->ToString(), nullptr, ( *it_end )->m_si );
		}
		elements.push_back( GetExpression( it, it_end ) );
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Array( GetSI( begin - 1, end + 1 ), elements );
}

const program::Object* JS::GetObject( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetObject" );
	std::unordered_map< std::string, const Expression* > properties = {};
	Identifier* identifier;
	source_elements_t::const_iterator it = begin, it_end;
	std::string property_key;
	while ( it != end ) {
		if ( ( *it )->m_type == SourceElement::ET_OPERATOR && ( (Operator*)( *it ) )->m_op == ":" ) {
			// empty key
			property_key = "";
		}
		else {
			// non-empty key
			identifier = ( Identifier * ) * it;
			if (
				( *it )->m_type != SourceElement::ET_IDENTIFIER ||
					identifier->m_identifier_type != IDENTIFIER_VARIABLE
				) {
				throw gse::Exception( EC.PARSE_ERROR, "Expected property key, got: " + ( *it )->ToString(), nullptr, ( *it )->m_si );
			}
			property_key = identifier->m_name;
			it++;
		}
		if (
			it == end ||
				( *it )->m_type != SourceElement::ET_OPERATOR ||
				( (Operator*)( *it ) )->m_op != ":"
			) {
			throw gse::Exception(
				EC.PARSE_ERROR, "Expected :" + ( it == end
					? " after property key"
					: ", got: " + ( *it )->ToString()
				), nullptr, ( *( it - 1 ) )->m_si
			);
		}
		it++;
		if ( it == end ) {
			throw gse::Exception( EC.PARSE_ERROR, "Expected property value after :", nullptr, ( *( it - 1 ) )->m_si );
		}

		// find property expression end
		for ( it_end = it ; it_end != end ; it_end++ ) {
			if ( ( *it_end )->m_type == SourceElement::ET_BLOCK ) {
				it_end = GetBracketsEnd( it_end, end );
			}
			else if (
				( *it_end )->m_type == SourceElement::ET_DELIMITER ) {
				if ( ( ( Delimiter * ) * it_end )->m_delimiter_type != Delimiter::DT_DATA ) {
					throw gse::Exception( EC.PARSE_ERROR, "Expected: , got: " + ( *it_end )->ToString(), nullptr, ( *it_end )->m_si );
				}
				break;
			}
		}
		if ( properties.find( property_key ) != properties.end() ) {
			throw gse::Exception( EC.PARSE_ERROR, "Duplicate property key: " + property_key, nullptr, ( *it_end )->m_si );
		}
		properties.insert_or_assign( property_key, GetExpression( it, it_end ) );
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Object( GetSI( begin - 1, end + 1 ), properties );
}

const JS::source_elements_t::const_iterator JS::GetBracketsEnd( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const {
	ASSERT( ( *begin )->m_type == SourceElement::ET_BLOCK, "expected block" );
	std::stack< uint8_t > brackets = {};
	source_elements_t::const_iterator it = begin;
	Block* block = ( Block * ) * begin;
	const auto& info = block->m_block_info;
	if ( block->m_block_side != Block::BS_BEGIN ) {
		int a = 5;
	}
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
					if ( brackets.empty() || brackets.top() != block->m_block_type ) {
						throw gse::Exception( EC.PARSE_ERROR, "Unexpected block close: " + block->ToString(), nullptr, block->m_si );
					}
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
	if ( it == end ) {
		throw gse::Exception( EC.PARSE_ERROR, (std::string)"Unclosed block: " + info.close_char + " not found", nullptr, ( *begin )->m_si );
	}
	return it;
}

const bool JS::IsObject( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const {
	return
		begin == end || (
			begin + 1 != end &&
				( *( begin + 1 ) )->m_type == SourceElement::ET_OPERATOR &&
				( (Operator*)( *( begin + 1 ) ) )->m_op == ":"
		);
}

#ifdef DEBUG

void JS::LogElement( const std::string& prefix, const SourceElement* element ) const {
	//Log( prefix + element->Dump() );
}

void JS::LogElements( const std::string& label, const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const {
	/*Log( label + "(" );
	for ( auto it = begin ; it != end ; it++ ) {
		LogElement( "    ", *it );
	}
	Log( ")" );*/
}

#endif

#undef ELS
#undef EL

}
}
