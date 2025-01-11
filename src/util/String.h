#pragma once

#include <vector>
#include <string>

#include "Util.h"

#include "types/Color.h"

namespace util {

CLASS( String, Util )

	static const std::vector< std::string > Split( const std::string& input, const char delimiter );

	static void Trim( std::string& s );
	static const std::string TrimCopy( const std::string& s );
	static const std::string ApproximateFloat( const float value );

	static const std::string ToUpperCase( const std::string& s );
	static const std::string ToLowerCase( const std::string& s );

	static const bool ParseInt( const std::string& s, long int& result );
	static const bool ParseFloat( const std::string& s, float& result );
	static const bool ParseColor( const std::string& s, types::Color& result );

};

}
