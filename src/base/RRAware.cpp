#include "RRAware.h"

// include every rr type here
#include "rr/GetData.h"
#include "rr/Capture.h"

namespace base {

template< class RRClass >
const id_t RRAware::RR_Send( RRClass* rr ) {
	id_t id = s_next_rr_id++;
	//Log( "RRing data at " + std::to_string( screen_x ) + "x" + std::to_string( screen_inverse_y ) );
	s_rrs[ id ] = rr;
	return id;
}

template< class RRClass >
RRClass* RRAware::RR_GetResponse( const id_t id ) {
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
void RRAware::RR_Cancel( const id_t id ) {
	auto it = s_rrs.find( id );
	if( it == s_rrs.end() ) {
		THROW( "invalid rr id" );
	}
	DELETE( (RRClass*)it->second );
	s_rrs.erase( it );
}

template< class RRClass >
std::vector< RRClass* > RRAware::RR_GetRequests() {
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
const bool RRAware::RR_HasRequests() {
	for ( auto& it : s_rrs ) {
		if ( !it.second->IsProcessed() && it.second->GetType() == RRClass::Type() ) {
			return true;
		}
	}
	return false;
}

// make sure to add every rr type here
INSTANTIATE( rr::GetData )
INSTANTIATE( rr::Capture )

}
