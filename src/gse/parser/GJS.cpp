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

namespace gse {
namespace parser {

using namespace program;

GJS::GJS( const std::string& source )
	: Parser( source ) {

}

void GJS::GetElements( source_elements_t& elements ) {
	char c;
	while ( !eof() ) {
		if ( match_sequence( "//", true ) ) {
			elements.push_back( new Comment( false, read_until_char_any( CHARS_EOLN.c_str(), false ) ) );
		}
		else if ( match_sequence( "/*", true ) ) {
			elements.push_back( new Comment( true, read_until_sequence( "*/", true ) ) );
		}
		else if ( ( c = match_char_any( CHARS_QUOTES.c_str(), true ) ) ) {
			elements.push_back( new Identifier( read_until_char( c, true ), IDENTIFIER_STRING ) );
		}
		else if ( match_char_any( CHARS_WHITESPACE.c_str(), true ) ) {
			skip_while_char_any( CHARS_WHITESPACE.c_str() );
		}
		else if ( match_char_any( CHARS_NUMBERS.c_str(), false ) ) {
			elements.push_back( new Identifier( read_while_char_any( CHARS_NUMBERS_C.c_str() ), IDENTIFIER_NUMBER ) );
		}
		else if ( match_char_any( CHARS_NAMES.c_str(), false ) ) {
			const auto value = read_while_char_any( CHARS_NAMES_C.c_str() );
			if ( KEYWORDS.find( value ) != KEYWORDS.end() ) {
				elements.push_back( new Operator( value ) );
			}
			else {
				elements.push_back( new Identifier( value, IDENTIFIER_VARIABLE ) );
			}
		}
		else if ( match_char_any( CHARS_OPERATORS.c_str(), false ) ) {
			elements.push_back( new Operator( read_while_char_any( CHARS_OPERATORS.c_str() ) ) );
		}
		else if ( ( c = match_char_any( CHARS_DELIMITERS.c_str(), true ) ) ) {
			switch ( c ) {
				case ';': {
					elements.push_back( new Control( Control::CT_FLOW_DELIMITER ) );
					break;
				}
				case ',': {
					elements.push_back( new Control( Control::CT_DATA_DELIMITER ) );
					break;
				}
				case '(': {
					elements.push_back( new Block( BLOCK_ROUND_BRACKETS, Block::BS_BEGIN ) );
					break;
				}
				case ')': {
					elements.push_back( new Block( BLOCK_ROUND_BRACKETS, Block::BS_END ) );
					break;
				}
				case '[': {
					elements.push_back( new Block( BLOCK_SQUARE_BRACKETS, Block::BS_BEGIN ) );
					break;
				}
				case ']': {
					elements.push_back( new Block( BLOCK_SQUARE_BRACKETS, Block::BS_END ) );
					break;
				}
				case '{': {
					elements.push_back( new Block( BLOCK_CURLY_BRACKETS, Block::BS_BEGIN ) );
					break;
				}
				case '}': {
					elements.push_back( new Block( BLOCK_CURLY_BRACKETS, Block::BS_END ) );
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

const program::Scope* GJS::GetScope( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetScope" )
	std::vector< const Statement* > body = {};
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
					( *it_end )->m_type == SourceElement::ET_CONTROL &&
					( ( Control * )( *it_end ) )->m_control_type == Control::CT_FLOW_DELIMITER
				) {
				if ( it != it_end ) {
					body.push_back( GetStatement( it, it_end ) );
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
	return new program::Scope( body );
}

const program::Statement* GJS::GetStatement( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetStatement" );
	return new program::Statement( GetExpression( begin, end ) );
}

const program::Operand* GJS::GetExpressionOrOperand( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end, const bool is_inside_object ) {
	ELS( "GetExpression" )
	// resolve elements, unpack scopes
	typedef std::vector< const Element* > elements_t;
	elements_t elements = {};
	source_elements_t::const_iterator it = begin, it_end, it_next, it_tmp;
	uint8_t t;
	bool var_hints_allowed = true;
	Variable::variable_hints_t next_var_hints = Variable::VH_NONE;
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
				const auto mod_it = MODIFIER_OPERATORS.find( ( (Operator*)( *it ) )->m_op );
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
				}
				else {
					const auto predef_it = PREDEF_OPERATORS.find( ( (Operator*)( *it ) )->m_op );
					if ( predef_it != PREDEF_OPERATORS.end() ) {
						elements.push_back( new program::Value( predef_it->second ) );
					}
					else {
						elements.push_back( GetOperator( (Operator*)( *it ) ) );
					}
					var_hints_allowed = false;
				}
				break;
			}
			case SourceElement::ET_CONTROL: {
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
						if ( !is_inside_object && it == begin ) {
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
											parameters.push_back( new Variable( ( ( Identifier * )( *it_tmp ) )->m_name ) );
										}
										else {
											ASSERT( false, "expected variable, got something else" );
										}
									}
									else {
										if (
											( *it_tmp )->m_type == SourceElement::ET_CONTROL &&
												( ( Control * ) * it_tmp )->m_control_type == Control::CT_DATA_DELIMITER
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
							elements.push_back( new program::Function( parameters, GetScope( it_next + 1, it_end ) ) );
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
										( *it_tmp )->m_type == SourceElement::ET_CONTROL &&
											( ( Control * )( *it_tmp ) )->m_control_type == Control::CT_DATA_DELIMITER
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
									operand->type == Operand::OT_EXPRESSION
										? (Expression*)operand
										: new Expression( operand ), arguments
								)
							);
						}
						break;
					}
					case BLOCK_SQUARE_BRACKETS: {
						// either array operator or array definition
						ASSERT( it != begin, "array syntax unexpected here" );
						switch ( (*( it - 1 ))->m_type ) {
							case SourceElement::ET_IDENTIFIER: {
								// array operator
								if (
									it_end != end &&
									it + 1 == it_end &&
									it_end + 1 != end &&
									(*(it_end + 1))->m_type == SourceElement::ET_OPERATOR &&
									((Operator*)(*(it_end + 1)))->m_op == "="
								) {
									// 'append' operator ( []= )
									elements.push_back( new program::Operator( program::Operator::OT_APPEND ) );
									it_end++;
								}
								else {
									// 'at' operator ( [i] )
									elements.push_back( new program::Operator( program::Operator::OT_AT ) );
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
	// split by operator priority
	const std::function< program::Operand*( const elements_t::const_iterator&, const elements_t::const_iterator& ) > get_operand = [ this, &get_operand, &elements ](
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
		for ( auto it = begin ; it != end ; it++ ) {
			if ( ( *it )->m_element_type == Element::ET_OPERATOR ) {
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

		switch ( link ) {
			case OL_LEFT: {
				ASSERT( has_a, "left operand is missing" );
				return new program::Expression( get_operand( begin, split_it ), (program::Operator*)*split_it );
			}
			case OL_RIGHT: {
				ASSERT( has_b, "right operand is missing" );
				return new program::Expression( nullptr, (program::Operator*)*split_it, get_operand( split_it + 1, end ) );
			}
			case OL_BOTH: {
				ASSERT( has_a, "left operand is missing" );
				ASSERT( has_b, "right operand is missing" );
				return new program::Expression( get_operand( begin, split_it ), (program::Operator*)*split_it, get_operand( split_it + 1, end ) );
			}
			case OL_ANY: {
				ASSERT( !has_a || !has_b, "both left and right operands are present" );
			case OL_ANY_OR_BOTH:
				ASSERT( has_a || has_b, "both left and right operands are missing" );
				return new program::Expression(
					has_a ? get_operand( begin, split_it ) : nullptr,
					(program::Operator*)*split_it,
					has_b ? get_operand( split_it + 1, end ) : nullptr
				);
			}
			default:
				ASSERT( false, "unexpected operator link type: " + std::to_string( link ) );
		}
	};

	return get_operand( elements.begin(), elements.end() );
}

const program::Expression* GJS::GetExpression( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end, const bool is_inside_object ) {
	const auto* operand = GetExpressionOrOperand( begin, end, is_inside_object );
	return operand->type == program::Operand::OT_EXPRESSION
		? (Expression*)operand
		: new Expression( operand );
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
			return new Variable( element->m_name, hints );
		}
		case IDENTIFIER_NUMBER: {
			try {
				// maybe it's int?
				const auto v = std::stoi( element->m_name.c_str() );
				return new program::Value( VALUE( type::Int, v ) );
			}
			catch ( std::logic_error const& ex ) {
				try {
					// maybe it's float?
					const auto f = std::stof( element->m_name.c_str() );
					return new program::Value( VALUE( type::Float, f ) );
				}
				catch ( std::logic_error const& ex ) {
					ASSERT( false, "value is not a number: " + element->m_name );
				}
			}
		}
		case IDENTIFIER_STRING: {
			return new program::Value( VALUE( type::String, element->m_name ) );
		}
		default:
			ASSERT( false, "unexpected identifier type: " + std::to_string( element->m_identifier_type ) );
	}
}

const program::Operator* GJS::GetOperator( const Operator* element ) {
	EL( "GetOperator" )
	const auto it = OPERATOR_NAMES.find( element->m_op );
	ASSERT( it != OPERATOR_NAMES.end(), "operator name not found: " + element->m_op );
	return new program::Operator( it->second );
}

const program::Array* GJS::GetArray( const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) {
	ELS( "GetArray" );
	std::vector< const Expression* > elements = {};
	Identifier* identifier;
	source_elements_t::const_iterator it = begin, it_end;
	while ( it != end ) {
		// find element expression end
		for ( it_end = it ; it_end != end ; it_end++ ) {
			if ( ( *it_end )->m_type == SourceElement::ET_BLOCK ) {
				it_end = GetBracketsEnd( it_end, end );
			}
			else if (
				( *it_end )->m_type == SourceElement::ET_CONTROL ) {
				ASSERT( ( ( Control * ) * it_end )->m_control_type == Control::CT_DATA_DELIMITER, "unexpected control symbol" );
				break;
			}
		}
		ASSERT( it_end != it, "expected element expression" );
		elements.push_back( GetExpression( it, it_end, true ) );
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Array( elements );
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
				( *it_end )->m_type == SourceElement::ET_CONTROL ) {
				ASSERT( ( ( Control * ) * it_end )->m_control_type == Control::CT_DATA_DELIMITER, "unexpected control symbol" );
				break;
			}
		}
		ASSERT( it_end != it, "expected property expression" );
		ASSERT( properties.find( identifier->m_name ) == properties.end(), "duplicate property key: " + identifier->m_name );
		properties.insert_or_assign( identifier->m_name, GetExpression( it, it_end, true ) );
		it = it_end;
		if ( it != end ) {
			it++;
		}
	}
	return new program::Object( properties );
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
	//Log( prefix + element->ToString() );
}

void GJS::LogElements( const std::string& label, const source_elements_t::const_iterator& begin, const source_elements_t::const_iterator& end ) const {
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
