#pragma once

#include <cstdint>
#include <vector>

#include "util/Util.h"

/**
 * Jenkins Small Fast 32-bit
 *   (can't use 64-bit for compatibility reasons, GLSMAC may run on 32-bit systems that connect to 64-bit host, etc)
 *   (can't use builtin C++ random classes because we need to be able to save and restore rng states)
 */

#include "Types.h"

namespace util {
namespace random {

CLASS( Random, Util )

	Random( const value_t seed = 0 );

	void SetSeed( const value_t seed );
	static const value_t NewSeed();

	static constexpr char s_state_divisor = ':';

	const bool GetBool();
	const uint32_t GetUInt( const uint32_t min = 0, const uint32_t max = UINT32_MAX - 1 );
	const int64_t GetInt64( int64_t min = INT64_MIN, int64_t = INT64_MAX - 1 );
	const float GetFloat( const float min = 0.0f, const float max = 1.0f );

	const bool IsLucky( const value_t difficulty = 2 ); // 2 means 50/50 chance

	template< class ValueType >
	void Shuffle( std::vector< ValueType >& vector );

	const state_t GetState();
	void SetState( const state_t& state );
	const std::string GetStateString();

	static const state_t GetStateFromString( std::string value );

private:

	state_t m_state = {};

	const value_t Generate();
};

}
}
