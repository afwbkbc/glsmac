#include "Random.h"

#include <random>
#include <climits>

namespace util {

Random::Random( const value_t seed ) {
	SetSeed( seed ? seed : NewSeed() );
}

#define rot32(x,k) (((x)<<(k))|((x)>>(32-(k))))
const Random::value_t Random::Generate() {
	// no lock guard, internal method
	
	value_t e = m_state.a - rot32( m_state.b, 27 );
	m_state.a = m_state.b ^ rot32(m_state.c, 17);
	m_state.b = m_state.c + m_state.d;
	m_state.c = m_state.d + e;
	m_state.d = e + m_state.a;
	return m_state.d;
}
#undef rot32

void Random::SetSeed( const value_t seed ) {
	lock_guard<mutex> guard( m_mutex );
	
	Log( "Setting seed " + to_string( seed ) );
	m_state.a = 0xf1ea5eed, m_state.b = m_state.c = m_state.d = seed;
	for ( value_t i = 0; i < 20; ++i ) {
		(void)Generate();
	}
	Log( "State set to " + GetStateStr() );
}

const Random::value_t Random::NewSeed() {
	std::random_device rd;
	return rd();
}

const bool Random::GetBool() {
	return (bool)GetUInt( 0, 1 );
}

const uint32_t Random::GetUInt( const uint32_t min, const uint32_t max ) {
	lock_guard<mutex> guard( m_mutex );
	
	ASSERT( max >= min, "GetUInt max larger than min" );
	
	value_t value = Generate();
	return min + value % ( max + 1 - min );
}

#define FLOAT_RANGE_MAX 100.0f

#define FLOAT_PRECISION ( (float)INT_MAX / FLOAT_RANGE_MAX )
const float Random::GetFloat( const float min, const float max ) {
	lock_guard<mutex> guard( m_mutex );
	
	ASSERT( max >= min, "GetFloat max larger than min" );
	
	ASSERT( min > -FLOAT_RANGE_MAX && min < FLOAT_RANGE_MAX, "GetFloat min overflow" );
	ASSERT( max > -FLOAT_RANGE_MAX && max < FLOAT_RANGE_MAX, "GetFloat max overflow" );
	
	value_t value = Generate();
	
	return (float)( min * FLOAT_PRECISION + value % (value_t)( ( max + 1 - min ) * FLOAT_PRECISION ) ) / FLOAT_PRECISION;
}
#undef FLOAT_PRECISION
#undef FLOAT_RANGE_MAX

const bool Random::IsLucky( const value_t difficulty ) {
	ASSERT( difficulty > 0, "IsLucky difficulty must be higher than 0" );
	
	value_t value = GetUInt( 0, difficulty - 1 );
	return value == 0;
}

const Random::state_t Random::GetState() {
	lock_guard<mutex> guard( m_mutex );
	
	return m_state;
}

void Random::SetState( const state_t& state ) {
	lock_guard<mutex> guard( m_mutex );
	
	m_state.a = state.a;
	m_state.b = state.b;
	m_state.c = state.c;
	m_state.d = state.d;
	Log( "State set to " + GetStateStr() );
}

const string Random::GetStateString() {
	lock_guard<mutex> guard( m_mutex );
	
	return GetStateStr();
}

const string Random::GetStateStr() {
	// no lock guard, internal method
	
	return "{" + to_string( m_state.a ) + "," + to_string( m_state.b ) + "," + to_string( m_state.c ) + "," + to_string( m_state.d ) + "}";
}

}
