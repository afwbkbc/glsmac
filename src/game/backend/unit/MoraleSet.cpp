#include "MoraleSet.h"

#include "Types.h"

namespace game {
namespace backend {
namespace unit {

MoraleSet::MoraleSet( const std::string& id, const morale_values_t& morale_values )
	: m_id( id )
	, m_morale_values( morale_values ) {
	ASSERT_NOLOG( MORALE_MIN == 0, "morale min should be zero for now" );
	ASSERT_NOLOG( morale_values.size() == MORALE_MAX - MORALE_MIN + 1, "morale set expects exactly " + std::to_string( MORALE_MAX - MORALE_MIN + 1 ) + " values" );
}

const std::string MoraleSet::ToString( const std::string& prefix ) const {
	std::string names = "";
	for ( size_t i = MORALE_MIN ; i <= MORALE_MAX ; i++ ) {
		if ( i != 0 ) {
			names += TS_ML_NEXT( m_morale_values.at( i ).m_name );
		}
	}
	return (std::string)
		TS_OBJ_BEGIN( "MoraleSet" ) +
		names +
		TS_OBJ_END();
}

const types::Buffer MoraleSet::Serialize( const MoraleSet* moraleset ) {
	ASSERT_NOLOG( MORALE_MIN == 0, "non-zero MORALE_MIN may cause unexpected bugs here" );
	types::Buffer buf;
	buf.WriteString( moraleset->m_id );
	for ( size_t i = MORALE_MIN ; i <= MORALE_MAX ; i++ ) {
		buf.WriteString( moraleset->m_morale_values.at( i ).m_name );
	}
	return buf;
}

MoraleSet* MoraleSet::Unserialize( types::Buffer& buf ) {
	ASSERT_NOLOG( MORALE_MIN == 0, "non-zero MORALE_MIN may cause unexpected bugs here" );
	const auto id = buf.ReadString();
	morale_values_t values = {};
	for ( size_t i = MORALE_MIN ; i <= MORALE_MAX ; i++ ) {
		values.push_back( Morale{ buf.ReadString() } );
	}
	return new MoraleSet( id, values );
}

}
}
}
