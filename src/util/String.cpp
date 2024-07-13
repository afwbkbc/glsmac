#include <cmath>
#include <sstream>
#include <algorithm>

#include "String.h"

namespace util {

const std::vector< std::string > String::SplitToLines( const std::string& input ) {
	std::vector< std::string > lines = {};
	auto ss = std::stringstream{ input };
	for ( std::string line ; std::getline( ss, line, '\n' ) ; ) {
		if ( !line.empty() && line.back() == '\r' ) {
			line.erase( line.end() - 1 );
		}
		lines.push_back( line );
	}
	return lines;
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

}
