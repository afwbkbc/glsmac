#pragma once

#include <vector>

#include "base/Base.h"

#include "gse/program/Program.h"
#include "gse/program/Scope.h"
#include "gse/program/Statement.h"
#include "gse/program/Object.h"

namespace gse {
namespace parser {

CLASS( Parser, base::Base )

	Parser( const std::string& source );

	const program::Program* Parse();

protected:

	const std::string CHARS_EOLN = "\r\n";
	const std::string CHARS_NUMBERS = "0123456789";
	const std::string CHARS_NUMBERS_C = CHARS_NUMBERS + ".";
	const std::string CHARS_LETTERS_LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
	const std::string CHARS_LETTERS_UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::string CHARS_LETTERS = CHARS_LETTERS_LOWERCASE + CHARS_LETTERS_UPPERCASE;

	class SourceElement {
	public:
		enum element_type_t {
			ET_COMMENT,
			ET_IDENTIFIER,
			ET_OPERATOR,
			ET_DELIMITER,
			ET_CONDITIONAL,
			ET_BLOCK,
		};
		SourceElement( const element_type_t type )
			: m_type( type ) {}
		const element_type_t m_type;
		virtual const std::string ToString() const = 0;
	};
	typedef std::vector< SourceElement* > source_elements_t;

	class Comment : public SourceElement {
	public:
		Comment( bool is_multiline, const std::string& text )
			: SourceElement( ET_COMMENT )
			, m_is_multiline( is_multiline )
			, m_text( text ) {}
		const bool m_is_multiline;
		const std::string m_text;
		virtual const std::string ToString() const {
			return "comment{ " + m_text + " }";
		}
	};

	class Identifier : public SourceElement {
	public:
		Identifier( const std::string& name, const uint8_t identifier_type )
			: SourceElement( ET_IDENTIFIER )
			, m_name( name )
			, m_identifier_type( identifier_type ) {}
		const std::string m_name;
		const uint8_t m_identifier_type;
		virtual const std::string ToString() const {
			return "identifier{ " + m_name + " }";
		}
	};

	class Operator : public SourceElement {
	public:
		Operator( const std::string& op )
			: SourceElement( ET_OPERATOR )
			, m_op( op ) {}
		const std::string m_op;
		virtual const std::string ToString() const {
			return "operator{ " + m_op + " }";
		};
	};

	class Delimiter : public SourceElement {
	public:
		enum delimiter_type_t {
			DT_FLOW,
			DT_DATA,
		};
		Delimiter( const delimiter_type_t delimiter_type )
			: SourceElement( ET_DELIMITER )
			, m_delimiter_type( delimiter_type ) {};

		const delimiter_type_t m_delimiter_type;

		virtual const std::string ToString() const {
			switch ( m_delimiter_type ) {
				case DT_FLOW:
					return "delimiter{ ; }";
				case DT_DATA:
					return "delimiter{ , }";
				default:
					ASSERT_NOLOG( false, "unexpected delimiter type: " + std::to_string( m_delimiter_type ) );
			}
		}
	};

	class Conditional : public SourceElement {
	public:
		enum conditional_type_t {
			CT_IF,
			CT_ELSEIF,
			CT_ELSE,
		};
		Conditional( const conditional_type_t conditional_type )
			: SourceElement( ET_CONDITIONAL )
			, m_conditional_type( conditional_type )
			, has_condition( conditional_type != CT_ELSE ) {};

		const conditional_type_t m_conditional_type;
		const bool has_condition;

		virtual const std::string ToString() const {
			switch ( m_conditional_type ) {
				case CT_IF:
					return "conditional{ if }";
				case CT_ELSEIF:
					return "conditional{ elseif }";
				case CT_ELSE:
					return "conditional{ else }";
				default:
					ASSERT_NOLOG( false, "unexpected conditional type: " + std::to_string( m_conditional_type ) );
			}
		}
	};

	class Block : public SourceElement {
	public:
		enum block_side_t {
			BS_BEGIN,
			BS_END,
		};
		Block( const uint8_t block_type, const block_side_t block_side )
			: SourceElement( ET_BLOCK )
			, m_block_type( block_type )
			, m_block_side( block_side ) {}
		const block_side_t m_block_side;
		const uint8_t m_block_type;
		virtual const std::string ToString() const {
			switch ( m_block_side ) {
				case BS_BEGIN:
					return "block_open{ " + std::to_string( m_block_type ) + " }";
				case BS_END:
					return "block_close{ " + std::to_string( m_block_type ) + " }";
				default:
					ASSERT_NOLOG( false, "unexpected block side: " + std::to_string( m_block_side ) );
			}
		}
	};

	virtual void GetElements( source_elements_t& elements ) = 0;
	virtual const program::Program* GetProgram( const source_elements_t& elements ) = 0;

	const char get() const; // get character at current position
	const bool eof() const; // returns true if source is parsed to the end

	// read and until end character encountered
	const std::string read_until_char( char chr, bool consume );
	// read until any of end characters encountered
	const std::string read_until_char_any( const char* chrs, bool consume );
	// read until end sequence encountered
	const std::string read_until_sequence( const char* sequence, bool consume );
	// read while any of characters match
	const std::string read_while_char_any( const char* chrs );
	// skip while any of characters match
	void skip_while_char_any( const char* chrs );

	// check if character occurs at current position
	const bool match_char( const char chr, bool consume );
	// check if any of characters occurs at current position
	const char match_char_any( const char* chrs, bool consume );
	// check if sequence occurs at current position
	const bool match_sequence( const char* sequence, bool consume );

private:
	const std::string m_source;

	const char* const m_begin;
	const char* const m_end;
	const char* m_ptr = nullptr;

};

}
}
