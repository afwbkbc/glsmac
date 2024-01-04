#include "Parser.h"

#include <cstring>

namespace gse {
namespace parser {

Parser::Parser( const std::string& filename, const std::string& source, const size_t initial_line_num )
	: m_source( source )
	, m_filename( filename )
	, m_begin( m_source.c_str() )
	, m_end( m_source.c_str() + m_source.size() ) {
	m_ptr = m_begin;
	m_si_pos.line = initial_line_num;
	m_si_pos.col = 1;
}

const program::Program* Parser::Parse() {
	source_elements_t elements = {};
	GetElements( elements );
	const auto* program = GetProgram( elements );
	for ( auto& it : elements ) {
		delete it;
	}
	return program;
}

const char Parser::get() const {
	ASSERT( m_ptr < m_end, "parser read overflow" );
	return *m_ptr;
}

const bool Parser::eof() const {
	return m_ptr == m_end;
}

const bool Parser::match_char( const char chr, bool consume ) {
	if ( m_ptr == m_end ) {
		return false;
	}
	else {
		if ( *m_ptr == chr ) {
			if ( consume ) {
				move();
			}
			return true;
		}
		return false;
	}
}

const char Parser::match_char_any( const char* chrs, bool consume ) {
	if ( m_ptr == m_end ) {
		return 0;
	}
	else {
		for ( const char* p = chrs ; *p ; p++ ) {
			if ( *m_ptr == *p ) {
				if ( consume ) {
					move();
				}
				return *p;
			}
		}
		return 0;
	}
}

const bool Parser::match_sequence( const char* sequence, bool consume ) {
	const size_t len = strlen( sequence );
	if ( m_ptr + len >= m_end ) {
		return false;
	}
	else {
		if ( !memcmp( m_ptr, sequence, len ) ) {
			if ( consume ) {
				move_by( len );
			}
			return true;
		}
		return false;
	}
}

const si_t::pos_t& Parser::get_si_pos() const {
	return m_si_pos;
}

const si_t Parser::get_si( const si_t::pos_t& begin, const si_t::pos_t& end ) const {
	return {
		m_filename,
		begin,
		end
	};
}

const std::string Parser::read_until_char( char chr, bool consume ) {
	const char* begin_ptr = m_ptr;
	while ( m_ptr < m_end - 1 && *m_ptr != chr ) {
		move();
	}
	const char* end_ptr = m_ptr;
	if ( consume ) {
		move();
	}
	return std::string( begin_ptr, end_ptr );
}

const std::string Parser::read_until_char_any( const char* chrs, bool consume ) {
	const char* begin_ptr = m_ptr;
	const char* p;
	while ( m_ptr < m_end - 1 ) {
		for ( p = chrs ; *p ; p++ ) {
			if ( *m_ptr == *p ) {
				break;
			}
		}
		if ( *p ) {
			break;
		}
		move();
	}
	if ( consume && m_ptr != m_end - 1 ) {
		move();
	}
	return std::string( begin_ptr, m_ptr );
}

const std::string Parser::read_until_sequence( const char* sequence, bool consume ) {
	const char* begin_ptr = m_ptr;
	const char* end = strchr( sequence, 0 );
	const char* p1;
	const char* p2;
	while ( m_ptr < m_end ) {
		p1 = m_ptr;
		p2 = sequence;
		while ( p1 < m_end && p2 < end ) {
			if ( *p1 != *p2 ) {
				break;
			}
			p1++;
			p2++;
		}
		if ( p2 == end ) {
			break;
		}
		move();
	}
	const char* end_ptr = m_ptr;
	if ( consume ) {
		move_by( std::min( end - sequence, m_end - m_ptr - 1 ) );
	}
	return std::string( begin_ptr, end_ptr );
}

const std::string Parser::read_while_char_any( const char* chrs ) {
	const char* begin_ptr = m_ptr;
	skip_while_char_any( chrs );
	return std::string( begin_ptr, m_ptr );
}

void Parser::skip_while_char_any( const char* chrs ) {
	const char* p;
	while ( m_ptr < m_end - 1 ) {
		for ( p = chrs ; *p ; p++ ) {
			if ( *m_ptr == *p ) {
				break;
			}
		}
		if ( !*p ) {
			break;
		}
		move();
	}
}

inline void Parser::move() {
	if ( *m_ptr == '\n' ) {
		m_si_pos.line++;
		m_si_pos.col = 1;
	}
	else {
		m_si_pos.col++;
	}
	m_ptr++;
}

inline void Parser::move_by( const size_t len ) {
	for ( size_t i = 0 ; i < len ; i++ ) {
		move();
	}
}

}
}
