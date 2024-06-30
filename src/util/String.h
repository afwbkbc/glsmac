#pragma once

#include <vector>
#include <string>

#include "Util.h"

namespace util {

CLASS( String, Util )

	static const std::vector< std::string > SplitToLines( const std::string& input );

	static void Trim( std::string& s );
	static const std::string TrimCopy( const std::string& s );
	static const std::string ApproximateFloat( const float value );

};

}
