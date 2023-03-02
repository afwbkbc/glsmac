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

}
