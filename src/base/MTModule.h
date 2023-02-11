#pragma once

#include <mutex>
#include <functional>
#include <unordered_map>

#include "Module.h"

namespace base {

typedef size_t mt_id_t;

static std::mutex s_next_mt_id_mutex;
static mt_id_t s_next_mt_id = 0;

template< typename REQUEST_TYPE, typename RESPONSE_TYPE >
class MTModule : public Module {
public:
		
	virtual void Iterate() {
		mt_request_map_t requests = MT_GetRequests();
		if ( !requests.empty() ) {
			//Log( "MT Processing " + to_string( requests.size() ) + " requests" );
			mt_response_map_t responses = {};
			for ( auto& request : requests ) {
				responses[ request.first ] = ProcessRequest( request.second );
			}
			MT_SetResponses( responses );
		}
	}
	
	void MT_Cancel( const mt_id_t mt_id ) {
		m_mt_states_mutex.lock();
		auto it = m_mt_states.find( mt_id );
		ASSERT( it != m_mt_states.end(), "MT_Cancel() mt_id not found" );
		if ( !it->second.is_executed && !it->second.is_processing ) {
			//Log( "MT Request " + to_string( mt_id ) + " canceled" );
			m_mt_states.erase( it );
		}
		m_mt_states_mutex.unlock();
	}

protected:
	
	// get request, return response
	virtual const RESPONSE_TYPE ProcessRequest( const REQUEST_TYPE& request ) = 0;
	
	// use these to pass data from/to other threads
	mt_id_t MT_CreateRequest( const REQUEST_TYPE& data ) {
		mt_state_t state = {};
		s_next_mt_id_mutex.lock();
		mt_id_t mt_id = ++s_next_mt_id;
		s_next_mt_id_mutex.unlock();
		state.is_executed = false;
		state.request = data;
		m_mt_states_mutex.lock();
		ASSERT( m_mt_states.find( mt_id ) == m_mt_states.end(), "duplicate mt_id" );
		m_mt_states[ mt_id ] = state;
		m_mt_states_mutex.unlock();
		//Log( "MT Request " + to_string( mt_id ) + " created" );
		return mt_id;
	}
	
	const RESPONSE_TYPE MT_GetResponse( const mt_id_t mt_id ) {
		RESPONSE_TYPE response = {};
		m_mt_states_mutex.lock();
		auto it = m_mt_states.find( mt_id );
		ASSERT( it != m_mt_states.end(), "GetResponse() mt_id not found" );
		if ( it->second.is_executed ) {
			response = it->second.response;
			m_mt_states.erase( it );
			//Log( "MT Request " + to_string( mt_id ) + " result returned" );
		}
		m_mt_states_mutex.unlock();

		return response;
	}		
	
private:
	
	typedef struct {
		REQUEST_TYPE request;
		bool is_processing;
		bool is_executed;
		RESPONSE_TYPE response;
	} mt_state_t;
	
	typedef std::unordered_map<mt_id_t, REQUEST_TYPE> mt_request_map_t;
	typedef std::unordered_map<mt_id_t, RESPONSE_TYPE> mt_response_map_t;
	
	mt_request_map_t MT_GetRequests() {
		mt_request_map_t result = {};
		m_mt_states_mutex.lock();
		for (auto& it : m_mt_states) {
			if ( !it.second.is_executed && !it.second.is_processing ) {
				it.second.is_processing = true;
				result[ it.first ] = it.second.request;
			}
		}
		m_mt_states_mutex.unlock();
		return result;
	}
		
	void MT_SetResponses( const mt_response_map_t& responses ) {
		if ( !responses.empty() ) {
			m_mt_states_mutex.lock();
			for (auto& response : responses) {
				auto it = m_mt_states.find( response.first );
				ASSERT( it != m_mt_states.end(), "invalid response mt_id" );
				ASSERT( it->second.is_processing, "setting response on non-processed request" );
				it->second.response = response.second;
				it->second.is_executed = true;
				it->second.is_processing = false;
				//Log( "MT Request " + to_string( response.first ) + " executed" );
			}
			m_mt_states_mutex.unlock();
		}
	}
	
	typedef std::unordered_map< mt_id_t, mt_state_t > mt_states_t;
	std::mutex m_mt_states_mutex;
	mt_states_t m_mt_states = {};
	
};

}
