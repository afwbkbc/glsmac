#pragma once

#include <unordered_map>

#include "rr/RR.h"

namespace base {

// Request-Response
class RRAware {
public:
	template< class RRClass >
	const id_t RR_Send( RRClass* rr );
	template< class RRClass >
	RRClass* RR_GetResponse( const id_t id );
	template< class RRClass >
	void RR_Cancel( const id_t id );
	template< class RRClass >
	std::vector< RRClass* > RR_GetRequests();
	template< class RRClass >
	const bool RR_HasRequests();
	
private:
	std::unordered_map< id_t, rr::RR* > m_rrs = {};
	id_t m_next_rr_id = 1;
	
};

// TODO: refactor?
#define INSTANTIATE( _rr ) \
	template const id_t RRAware::RR_Send< _rr >( _rr* rr ); \
	template _rr* RRAware::RR_GetResponse< _rr >( const id_t id ); \
	template void RRAware::RR_Cancel<_rr>( const id_t id ); \
	template std::vector< _rr* > RRAware::RR_GetRequests< _rr >(); \
	template const bool RRAware::RR_HasRequests< _rr >();


}
