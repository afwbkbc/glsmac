#pragma once

#include "Util.h"

/**
 * Jenkins Small Fast 32-bit
 *   (can't use 64-bit for compatibility reasons, GLSMAC may run on 32-bit systems that connect to 64-bit host, etc)
 *   (can't use builtin C++ random classes because we need to be able to save and restore rng states)
 */

namespace util {

CLASS( Random, Util )
	
	typedef uint32_t value_t;
	
	Random( const value_t seed = 0 );
	
	void SetSeed( const value_t seed );
	static const value_t NewSeed();
	
	struct state_t {
		value_t a;
		value_t b;
		value_t c;
		value_t d;
	};
	
	const bool GetBool();
	const uint32_t GetUInt( const uint32_t min, const uint32_t max );
	const float GetFloat( const float min, const float max );
	
	const bool IsLucky( const value_t difficulty );
	
	const state_t GetState();
	void SetState( const state_t& state );
	const std::string GetStateString();
	
private:
	
	state_t m_state = {};
	
	const value_t Generate();
};

}
