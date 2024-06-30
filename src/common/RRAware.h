#pragma once

#include <vector>
#include <unordered_map>

#include "rr/Types.h"

namespace rr {
class RR;
}

namespace common {

// Request-Response
class RRAware {
public:
	template< class RRClass >
	const rr::id_t RR_Send( RRClass* rr );

	template< class RRClass >
	RRClass* RR_GetResponse( const rr::id_t id );

	template< class RRClass >
	void RR_Cancel( const rr::id_t id );

	template< class RRClass >
	std::vector< RRClass* > RR_GetRequests();

	template< class RRClass >
	const bool RR_HasRequests();

private:
	std::unordered_map< rr::id_t, rr::RR* > m_rrs = {};

	rr::id_t m_next_rr_id = 1;

};

// TODO: refactor?
#define INSTANTIATE( _rr ) \
    template const rr::id_t RRAware::RR_Send< _rr >( _rr* rr ); \
    template _rr* RRAware::RR_GetResponse< _rr >( const rr::id_t id ); \
    template void RRAware::RR_Cancel<_rr>( const rr::id_t id ); \
    template std::vector< _rr* > RRAware::RR_GetRequests< _rr >(); \
    template const bool RRAware::RR_HasRequests< _rr >();

}
