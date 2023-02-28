#include <unordered_map>

#include "RR.h"

#include "GetData.h"

namespace rr {

RR::RR( const type_t type )
	: m_type( type )
{
	//Log( "RR created" );
}

RR::~RR() {
	//Log( "RR destroyed" );
}

const type_t RR::GetType() const {
	return m_type;
}

const bool RR::IsProcessed() const {
	return m_is_processed;
}

void RR::SetProcessed() {
	ASSERT( !m_is_processed, "rr already processed" );
	//Log( "RR processed" );
	m_is_processed = true;
}

// static API

thread_local static std::unordered_map< id_t, RR* > s_rrs = {};
thread_local static id_t s_next_rr_id = 1;

template< class RRClass >
const id_t Send( RRClass* rr ) {
	id_t id = s_next_rr_id++;
	//Log( "RRing data at " + std::to_string( screen_x ) + "x" + std::to_string( screen_inverse_y ) );
	s_rrs[ id ] = rr;
	return id;
}

template< class RRClass >
RRClass* GetResponse( const id_t id ) {
	auto it = s_rrs.find( id );
	if ( it == s_rrs.end() ) {
		THROW( "invalid rr id" );
	}
	if ( it->second->IsProcessed() ) {
		auto* r = (RRClass*)it->second;
		s_rrs.erase( it );
		return r;
	}
	else {
		return nullptr;
	}
}

template< class RRClass >
void Cancel( const id_t id ) {
	auto it = s_rrs.find( id );
	if( it == s_rrs.end() ) {
		THROW( "invalid rr id" );
	}
	DELETE( (RRClass*)it->second );
	s_rrs.erase( it );
}

template< class RRClass >
std::vector< RRClass* > GetRequests() {
	std::vector< RRClass* > rrs = {};
	for ( auto& it : s_rrs ) {
		if ( !it.second->IsProcessed() && it.second->GetType() == RRClass::Type() ) {
			auto* r = (RRClass*)it.second;
			rrs.push_back( r );
		}
	}
	return rrs;
}

template< class RRClass >
const bool HasRequests() {
	for ( auto& it : s_rrs ) {
		if ( !it.second->IsProcessed() && it.second->GetType() == RRClass::Type() ) {
			return true;
		}
	}
	return false;
}

	INSTANTIATE( GetData )

}
