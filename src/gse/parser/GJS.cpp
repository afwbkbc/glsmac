#include "GJS.h"

#include "gse/type/Int.h"
#include "gse/type/String.h"

#include "gse/program/Variable.h"
#include "gse/program/Value.h"
#include "gse/program/Object.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"

namespace gse {
namespace parser {

using namespace program;

GJS::GJS( const std::string& source )
	: Parser( source ) {

}

const program::Program* GJS::GetProgram( const std::vector< SourceElement* >& elements ) {
	// TODO
	NEWV( program, program::Program, new program::Scope( {} ) );
	return program;
}

void GJS::GetElements( std::vector< SourceElement* >& elements ) {
	char c;
	while ( !eof() ) {
		if ( match_sequence( "//", true ) ) {
			elements.push_back( new Comment( false, read_until_char_any( CHARS_EOLN.c_str(), false ) ) );
		}
		else if ( match_sequence( "/*", true ) ) {
			elements.push_back( new Comment( true, read_until_sequence( "*/", true ) ) );
		}
		else if ( match_char_any( CHARS_WHITESPACE.c_str(), true ) ) {
			skip_while_char_any( CHARS_WHITESPACE.c_str() );
		}
		else if ( match_char_any( CHARS_NUMBERS.c_str(), false ) ) {
			elements.push_back( new Identifier( read_while_char_any( CHARS_NUMBERS_C.c_str() ), IDENTIFIER_NUMBER ) );
		}
		else if ( match_char_any( CHARS_NAMES.c_str(), false ) ) {
			elements.push_back( new Identifier( read_while_char_any( CHARS_NAMES_C.c_str() ), IDENTIFIER_VARIABLE ) );
		}
		else if ( ( c = match_char_any( CHARS_QUOTES.c_str(), true ) ) ) {
			elements.push_back( new Identifier( read_until_char( c, true ), IDENTIFIER_STRING ) );
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

}
}
