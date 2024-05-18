#pragma once

#include <vector>
#include <cstdint>

#include "base/Base.h"

#include "gse/Types.h"

namespace gse {

namespace program {
class Program;
}

namespace parser {

CLASS( Parser, base::Base )

	Parser( const std::string& filename, const std::string& source, const size_t initial_line_num );
	virtual ~Parser() = default;

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
		SourceElement( const element_type_t type, const si_t& si )
			: m_type( type )
			, m_si( si ) {}
		virtual ~SourceElement() = default;
		const element_type_t m_type;
		const si_t m_si;
		virtual const std::string ToString() const = 0;
		virtual const std::string Dump() const = 0;
	};
	typedef std::vector< SourceElement* > source_elements_t;

	class Identifier : public SourceElement {
	public:
		Identifier( const std::string& name, const uint8_t identifier_type, const si_t& si )
			: SourceElement( ET_IDENTIFIER, si )
			, m_name( name )
			, m_identifier_type( identifier_type ) {}
		const std::string m_name;
		const uint8_t m_identifier_type;
		const std::string ToString() const override {
			return m_name;
		}
		const std::string Dump() const override {
			return m_si.ToString() + "identifier{" + m_name + "}";
		}
	};

	class Operator : public SourceElement {
	public:
		Operator( const std::string& op, const si_t& si )
			: SourceElement( ET_OPERATOR, si )
			, m_op( op ) {}
		const std::string m_op;
		const std::string ToString() const override {
			return m_op;
		};
		const std::string Dump() const override {
			return m_si.ToString() + "operator{" + m_op + "}";
		};
	};

	class Delimiter : public SourceElement {
	public:
		enum delimiter_type_t {
			DT_CODE,
			DT_DATA,
		};
		Delimiter( const delimiter_type_t delimiter_type, const si_t& si )
			: SourceElement( ET_DELIMITER, si )
			, m_delimiter_type( delimiter_type ) {};

		const delimiter_type_t m_delimiter_type;

		const std::string ToString() const override {
			switch ( m_delimiter_type ) {
				case DT_CODE:
					return ";";
				case DT_DATA:
					return ",";
				default:
					THROW( "unexpected delimiter type: " + std::to_string( m_delimiter_type ) );
			}
		}
		const std::string Dump() const override {
			switch ( m_delimiter_type ) {
				case DT_CODE:
					return m_si.ToString() + "delimiter{code}";
				case DT_DATA:
					return m_si.ToString() + "delimiter{data}";
				default:
					THROW( "unexpected delimiter type: " + std::to_string( m_delimiter_type ) );
			}
		}
	};

	class Conditional : public SourceElement {
	public:
		enum conditional_type_t {
			CT_IF,
			CT_ELSEIF,
			CT_ELSE,
			CT_WHILE,
			CT_TRY,
			CT_CATCH,
		};
		Conditional( const conditional_type_t conditional_type, const si_t& si )
			: SourceElement( ET_CONDITIONAL, si )
			, m_conditional_type( conditional_type )
			, has_condition(
				conditional_type == CT_IF ||
					conditional_type == CT_ELSEIF ||
					conditional_type == CT_WHILE
			) {};

		const conditional_type_t m_conditional_type;
		const bool has_condition;

		const std::string ToString() const override {
			switch ( m_conditional_type ) {
				case CT_IF:
					return "if";
				case CT_ELSEIF:
					return "elseif";
				case CT_ELSE:
					return "else";
				case CT_WHILE:
					return "while";
				case CT_TRY:
					return "try";
				case CT_CATCH:
					return "catch";
				default:
					THROW( "unexpected conditional type: " + std::to_string( m_conditional_type ) );
			}
		}
		const std::string Dump() const override {
			return m_si.ToString() + "conditional{" + ToString() + "}";
		}
	};

	class Block : public SourceElement {
	public:
		enum block_side_t {
			BS_BEGIN,
			BS_END,
		};
		struct block_info_t {
			uint8_t type;
			char open_char;
			char close_char;
		};
		Block( const block_info_t& block_info, const block_side_t block_side, const si_t& si )
			: SourceElement( ET_BLOCK, si )
			, m_block_info( block_info )
			, m_block_type( block_info.type )
			, m_block_side( block_side ) {}
		const block_info_t m_block_info;
		const uint8_t m_block_type;
		const block_side_t m_block_side;
		const std::string ToString() const override {
			switch ( m_block_side ) {
				case BS_BEGIN:
					return std::string( 1, m_block_info.open_char );
				case BS_END:
					return std::string( 1, m_block_info.close_char );
				default:
					THROW( "unexpected block side: " + std::to_string( m_block_side ) );
			}
		}
		const std::string Dump() const override {
			switch ( m_block_side ) {
				case BS_BEGIN:
					return m_si.ToString() + "block_open{" + std::to_string( m_block_type ) + "}";
				case BS_END:
					return m_si.ToString() + "block_close{" + std::to_string( m_block_type ) + "}";
				default:
					THROW( "unexpected block side: " + std::to_string( m_block_side ) );
			}
		}
	};

	virtual void GetElements( source_elements_t& elements ) = 0;
	virtual const program::Program* GetProgram( const source_elements_t& elements ) = 0;

	const char get() const; // get character at current position
	const bool eof() const; // returns true if source is parsed to the end

	// read and until end character encountered
	const std::string read_until_char( char chr, bool consume, bool handle_backslashes = false );
	// read until any of end characters encountered
	const std::string read_until_char_any( const char* chrs, bool consume );
	// read until end sequence encountered
	const std::string read_until_sequence( const char* sequence, bool consume );
	// read while any of characters match
	const std::string read_while_char_any( const char* chrs );
	// skip while any of characters match
	void skip_while_char_any( const char* chrs );
	// skip until any of characters match
	void skip_until_char_any( const char* chrs, bool consume );
	// skip until end sequence encountered
	void skip_until_sequence( const char* sequence, bool consume );

	// check if character occurs at current position
	const bool match_char( const char chr, bool consume );
	// check if any of characters occurs at current position
	const char match_char_any( const char* chrs, bool consume );
	// check if sequence occurs at current position
	const bool match_sequence( const char* sequence, bool consume );

	// check next coming char and does not modify current position
	const char check_char_any( const char* chrs );

	// returns last recorded source info
	const si_t::pos_t& get_si_pos() const;

	// creates si object with provided position
	const si_t make_si( const si_t::pos_t& begin, const si_t::pos_t& end ) const;

	const std::string unpack_backslashes( const std::string& source ) const;

private:
	const std::string m_source;
	const std::string m_filename;

	const char* const m_begin;
	const char* const m_end;
	const char* m_ptr = nullptr;

	si_t::pos_t m_si_pos = {};

	inline void move();
	inline void move_by( const size_t len = 1 );
};

}
}
