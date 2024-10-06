#include <random>
#include <algorithm>
#include <climits>

#include "Random.h"

#include "game/backend/map/tile/Tile.h"

namespace util {
namespace random {

Random::Random( const value_t seed ) {
	SetSeed(
		seed
			? seed
			: NewSeed()
	);
}

#define rot32( x, k ) (((x)<<(k))|((x)>>(32-(k))))

const value_t Random::Generate() {
	value_t e = m_state.a - rot32( m_state.b, 27 );
	m_state.a = m_state.b ^ rot32( m_state.c, 17 );
	m_state.b = m_state.c + m_state.d;
	m_state.c = m_state.d + e;
	m_state.d = e + m_state.a;
	return m_state.d;
}

#undef rot32

void Random::SetSeed( const value_t seed ) {
	//Log( "Setting seed " + std::to_string( seed ) );
	m_state.a = 0xf1ea5eed, m_state.b = m_state.c = m_state.d = seed;
	for ( value_t i = 0 ; i < 20 ; ++i ) {
		(void)Generate();
	}
	Log( "State set to " + GetStateString() );
}

const value_t Random::NewSeed() {
	std::random_device rd;
	return rd();
}

const bool Random::GetBool() {
	return (bool)GetUInt( 0, 1 );
}

const uint32_t Random::GetUInt( const uint32_t min, const uint32_t max ) {
	ASSERT( max >= min, "GetUInt min larger than max" );

	value_t value = Generate();

	return min + value % ( max - min + 1 );
}

const int64_t Random::GetInt64( int64_t min, int64_t max ) {
	ASSERT( max >= min, "GetInt64 min larger than max" );

	return min + llabs( (int64_t)( ( (uint64_t)GetUInt() << 32 ) | GetUInt() ) ) % ( max - min + 1 );
}

#define FLOAT_RANGE_MAX 100.0f

#define FLOAT_PRECISION ( (float)INT_MAX / FLOAT_RANGE_MAX )

const float Random::GetFloat( const float min, const float max ) {
	ASSERT( max >= min, "GetFloat max larger than min" );

	ASSERT( min > -FLOAT_RANGE_MAX && min < FLOAT_RANGE_MAX, "GetFloat min range overflow" );
	ASSERT( max > -FLOAT_RANGE_MAX && max < FLOAT_RANGE_MAX, "GetFloat max range overflow" );

	value_t value = Generate();

	const float small_value = 0.00001f;

	float ret = (float)( ( min + small_value ) * FLOAT_PRECISION + value % (value_t)( ( max - min - small_value * 2 ) * FLOAT_PRECISION ) ) / FLOAT_PRECISION;
	ASSERT( ret >= min, "GetFloat ret < min ( " + std::to_string( ret ) + " < " + std::to_string( min ) + " )" );
	ASSERT( ret <= max, "GetFloat ret > max ( " + std::to_string( ret ) + " > " + std::to_string( max ) + " )" );
	return ret;
}

#undef FLOAT_PRECISION
#undef FLOAT_RANGE_MAX

const bool Random::IsLucky( const value_t difficulty ) {
	ASSERT( difficulty > 0, "IsLucky difficulty must be higher than 0" );

	value_t value = GetUInt( 0, difficulty - 1 );
	return value == 0;
}

template< class ValueType >
void Random::Shuffle( std::vector< ValueType >& vector ) {
	std::mt19937 g( GetUInt() );
	std::shuffle( vector.begin(), vector.end(), g );
}

const state_t Random::GetState() {
	return m_state;
}

void Random::SetState( const state_t& state ) {
	m_state.a = state.a;
	m_state.b = state.b;
	m_state.c = state.c;
	m_state.d = state.d;
	Log( "State set to " + GetStateString() );
}

const std::string Random::GetStateString() {
	return std::to_string( m_state.a ) + s_state_divisor + std::to_string( m_state.b ) + s_state_divisor + std::to_string( m_state.c ) + s_state_divisor + std::to_string( m_state.d );
}

const state_t Random::GetStateFromString( std::string value ) {
	state_t state = {};
	size_t pos = 0;
	const std::string s_invalid_state_format = "Invalid state format";
	const auto f_trynext = [ &value, &pos, &s_invalid_state_format ]( value_t* ptr ) -> void {
		pos = value.find( s_state_divisor );
		if ( pos == std::string::npos ) {
			THROW( s_invalid_state_format );
		}
		try {
			*ptr = std::stoul( value.substr( 0, pos ) );
		}
		catch ( std::invalid_argument& e ) {
			THROW( s_invalid_state_format );
		}
		value = value.substr( pos + 1 );
	};
	f_trynext( &state.a );
	f_trynext( &state.b );
	f_trynext( &state.c );
	try {
		state.d = std::stoul( value );
	}
	catch ( std::invalid_argument& e ) {
		THROW( s_invalid_state_format );
	}
	return state;
}

template void Random::Shuffle< game::backend::map::tile::Tile* >( std::vector< game::backend::map::tile::Tile* >& vector );

}
}

