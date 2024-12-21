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

const std::vector< std::string > String::Split( const std::string& input, const char delimiter ) {
	std::vector< std::string > result = {};
	auto ss = std::stringstream{ input };
	for ( std::string part ; std::getline( ss, part, delimiter ) ; ) {
		if ( delimiter == '\n' && !part.empty() && part.back() == '\r' ) {
			part.erase( part.end() - 1 );
		}
		result.push_back( part );
	}
	return result;
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

const bool String::ParseColor( const std::string& s, types::Color& color ) {
	if ( s.empty() || s.at( 0 ) != '#' ) {
		return false;
	}
	const auto has_alpha = s.length() == 9;
	if ( !has_alpha && s.length() != 7 ) {
		return false;
	}
	const auto& str = s.c_str();
	for ( const char* c = str + 1 ; *c ; c++ ) {
		const auto cc = *c;
		if ( ( *c < '0' || *c > '9' ) && ( *c < 'a' || *c > 'f' ) && ( *c < 'A' || *c > 'F' ) ) {
			return false;
		}
	}
	color.value.red = gethexfloat( str + 1 );
	color.value.green = gethexfloat( str + 3 );
	color.value.blue = gethexfloat( str + 5 );
	color.value.alpha = has_alpha
		? gethexfloat( str + 7 )
		: 1.0f;
	return true;
}

}
