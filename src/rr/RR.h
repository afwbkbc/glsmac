#pragma once

#include <vector>

#include "base/Base.h"

namespace rr {

enum type_t {
	RR_NONE,
	RR_GETDATA,
};

// Request-Response
CLASS( RR, base::Base )
	
	const type_t GetType() const;
	const bool IsProcessed() const;
	void SetProcessed();
	
	RR() = delete;
	virtual ~RR();
	
protected:
	RR( const type_t type );

private:
	const type_t m_type = RR_NONE;
	bool m_is_processed = false;
	
};

typedef size_t id_t;

// static API
// per-thread. that means request won't reach something running in different thread
// TODO: timeouts?

template< class RRClass >
const id_t Send( RRClass* rr );
template< class RRClass >
RRClass* GetResponse( const id_t id );
template< class RRClass >
void Cancel( const id_t id );
template< class RRClass >
std::vector< RRClass* > GetRequests();
template< class RRClass >
const bool HasRequests();

// TODO: refactor?
#define INSTANTIATE( _rr ) \
	template const id_t Send< _rr >( _rr* rr ); \
	template _rr* GetResponse< _rr >( const id_t id ); \
	template void Cancel<_rr>( const id_t id ); \
	template std::vector< _rr* > GetRequests< _rr >(); \
	template const bool HasRequests< _rr >();

}
