#pragma once

#include "Parser.h"

namespace gse {
namespace parser {

CLASS( GJS, Parser )

	GJS( const std::string& source );

protected:
	void GetElements( std::vector< SourceElement* >& elements ) override;
	const program::Program* GetProgram( const std::vector< SourceElement* >& elements ) override;

private:

	const std::string CHARS_WHITESPACE = CHARS_EOLN + "	 ";
	const std::string CHARS_NAMES = CHARS_LETTERS + "_";
	const std::string CHARS_NAMES_C = CHARS_NAMES + CHARS_NUMBERS;
	const std::string CHARS_OPERATORS = "=+-:<>*/.";
	const std::string CHARS_OPERATORS_C = CHARS_OPERATORS + ".";
	const std::string CHARS_QUOTES = "'\"";
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

};

}
}
