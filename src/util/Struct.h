#pragma once

#include <unordered_map>

#include "Util.h"

namespace util {

CLASS( Struct, Util )

	template< typename A, typename B >
	static const std::unordered_map< B, A > FlipMap( const std::unordered_map< A, B >& source ) {
		std::unordered_map< B, A > result = {};
		result.reserve( source.size() );
		for ( const auto& it : source ) {
			result.insert(
				{
					it.second,
					it.first
				}
			);
		}
		return result;
	}

};

}
