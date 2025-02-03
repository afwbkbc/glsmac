#include <cmath>
#include <sstream>
#include <algorithm>

#include "String.h"

namespace util {

inline const unsigned char gethex( const char* str ) {
	return (
		( *str >= 'a' )
			? ( *str - 'a' + 10 )
			:
			( *str >= 'A' )
				? ( *str - 'A' + 10 )
				: ( *str - '0' )
	) << 4 | (
		( *( str + 1 ) >= 'a' )
			? ( *( str + 1 ) - 'a' + 10 )
			:
			( *( str + 1 ) >= 'A' )
				? ( *( str + 1 ) - 'A' + 10 )
				: ( *( str + 1 ) - '0' )
	);
}

inline const float gethexfloat( const char* str ) {
	return (float)gethex( str ) / 255.0f;
}

// trim from start (in place)
static inline void ltrim( std::string& s ) {
	s.erase(
		s.begin(), std::find_if(
			s.begin(), s.end(), []( unsigned char ch ) {
				return !std::isspace( ch );
			}
		)
	);
}

// trim from end (in place)
static inline void rtrim( std::string& s ) {
	s.erase(
		std::find_if(
			s.rbegin(), s.rend(), []( unsigned char ch ) {
				return !std::isspace( ch );
			}
		).base(), s.end()
	);
}

// trim from both ends (in place)
static inline void trim( std::string& s ) {
	rtrim( s );
	ltrim( s );
}

const std::string& String::EOLN() {
	static const std::string s_eoln =
#ifdef _WIN32
		"\r\n"
#else
		"\n"
#endif
	;
	return s_eoln;
}

const std::vector< std::string > String::Split( const std::string& input, const char delimiter ) {
	std::vector< std::string > result = {};
	auto ss = std::stringstream{ input };
	for ( std::string part ; std::getline( ss, part, delimiter ) ; ) {
		if ( delimiter == '\n' && !part.empty() && part.back() == '\r' ) {
			part.erase( part.end() - 1 );
		}
		if ( delimiter == ',' ) {
			trim( part );
		}
		result.push_back( part );
	}
	return result;
}

const std::string String::Join( const std::vector< std::string >& input, const char delimiter ) {
	std::string result = "";
	for ( const auto& s : input ) {
		if ( !result.empty() ) {
			result.push_back( delimiter );
		}
		result += s;
	}
	return result;
}

void String::Trim( std::string& s ) {
	trim( s );
}

const std::string String::TrimCopy( const std::string& s ) {
	std::string str = s;
	trim( str );
	return str;
}

const std::string String::ApproximateFloat( const float value ) {
	std::string result = std::to_string( std::floor( value * 100 ) / 100 );
	result.erase( result.find_last_not_of( '0' ) + 1, std::string::npos );
	result.erase( result.find_last_not_of( '.' ) + 1, std::string::npos );
	return result;
}

const std::string String::ToUpperCase( const std::string& s ) {
	std::string result = s;
	std::transform( s.begin(), s.end(), result.begin(), ::toupper );
	return result;
}

const std::string String::ToLowerCase( const std::string& s ) {
	std::string result = s;
	std::transform( s.begin(), s.end(), result.begin(), ::tolower );
	return result;
}

static const std::unordered_set< char > s_printable = {
	'a',
	's',
	'd',
	'f',
	'g',
	'h',
	'j',
	'k',
	'l',
	'q',
	'w',
	'e',
	'r',
	't',
	'y',
	'u',
	'i',
	'o',
	'p',
	'z',
	'x',
	'c',
	'v',
	'b',
	'n',
	'm',
	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',
	'`',
	'-',
	'=',
	'~',
	'@',
	'#',
	'$',
	'%',
	'^',
	'&',
	'*',
	'(',
	')',
	'_',
	'+',
	'[',
	']',
	'\\',
	'{',
	'}',
	'|',
	';',
	'\'',
	':',
	'"',
	'.',
	'/',
	'<',
	'>',
	'?'
};
const std::string String::FilterPrintable( const std::string& s ) {
	// TODO: improve
	auto* r = (char*)malloc( s.length() + 1 );
	auto* cc = r;
	for ( const char* c = s.c_str() ; *c ; c++ ) {
		if ( s_printable.find( *c ) != s_printable.end() ) {
			*( cc++ ) = *c;
		}
	}
	*cc = '\0';
	std::string result = r;
	free( r );
	return result;
}

const bool String::ParseInt( const std::string& s, long int& result ) {
	char* r = nullptr;
	result = strtol( s.c_str(), &r, 10 );
	return r && *r == '\0';
}

const bool String::ParseFloat( const std::string& s, float& result ) {
	char* r = nullptr;
	result = strtof( s.c_str(), &r );
	return r && *r == '\0';
}

const bool String::ParseColorHex( const std::string& s, types::Color& result ) {
	if ( s.empty() || s.at( 0 ) != '#' ) {
		return false;
	}
	const auto has_alpha = s.length() == 9;
	if ( !has_alpha && s.length() != 7 ) {
		return false;
	}
	const auto& str = s.c_str();
	for ( const char* c = str + 1 ; *c ; c++ ) {
		if ( ( *c < '0' || *c > '9' ) && ( *c < 'a' || *c > 'f' ) && ( *c < 'A' || *c > 'F' ) ) {
			return false;
		}
	}
	result.value.red = gethexfloat( str + 1 );
	result.value.green = gethexfloat( str + 3 );
	result.value.blue = gethexfloat( str + 5 );
	result.value.alpha = has_alpha
		? gethexfloat( str + 7 )
		: 1.0f;
	return true;
}

}
