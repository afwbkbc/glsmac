#include "ThreadCall.h"

namespace types {

template< typename REQUESTDATATYPE, typename RESPONSEDATATYPE >
void ThreadCall<REQUESTDATATYPE, RESPONSEDATATYPE>::SetRequest( REQUESTDATATYPE ) {
	ASSERT( !m_is_executed, "request set after execution" );
}

template< typename REQUESTDATATYPE, typename RESPONSEDATATYPE >
bool ThreadCall<REQUESTDATATYPE, RESPONSEDATATYPE>::IsExecuted() const {
	return m_is_executed;
}

template< typename REQUESTDATATYPE, typename RESPONSEDATATYPE >
const RESPONSEDATATYPE ThreadCall<REQUESTDATATYPE, RESPONSEDATATYPE>::GetResponse() const {
	ASSERT( m_is_executed, "response queries before execution" );
	
}

}

