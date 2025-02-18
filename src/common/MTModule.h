#pragma once

#include <mutex>
#include <atomic>
#include <functional>
#include <map>
#include <thread>

#include "Module.h"
#include "MTTypes.h"

namespace common {

static std::mutex s_next_mt_id_mutex;
static mt_id_t s_next_mt_id = 0;

// requests and responses should be structs that contain operation type and unions of variables for every op type
// if you need to pass something non-trivial - use raw pointers
// recommended way to use raw pointers here:
//   create/malloc objects when creating request, delete/free in target thread when processing it
//   for response it's opposite - create/malloc when creating response, delete/free in original thread when reading response

template< typename REQUEST_TYPE, typename RESPONSE_TYPE >
class MTModule : public Module {
public:

	virtual void Iterate() {
		ProcessRequests();
	}

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
			DestroyRequest( it->second.request );
			m_mt_states.erase( it );
			//Log( "MT Request " + to_string( mt_id ) + " result returned" );
		}
		m_mt_states_mutex.unlock();

		return response;
	}

	// TODO: better way?
	void MT_DestroyResponse( const RESPONSE_TYPE& response ) {
		DestroyResponse( response );
	}

	void MT_Cancel( const mt_id_t mt_id ) {
		m_mt_states_mutex.lock();
		auto it = m_mt_states.find( mt_id );
		ASSERT( it != m_mt_states.end(), "MT_Cancel() mt_id not found" );
		if ( mt_id == m_current_request_id ) {
			m_is_canceled = true;
		}
		if ( !it->second.is_executed ) {
			if ( it->second.is_processing ) {
				Log( "Waiting for MT Request " + std::to_string( mt_id ) + " to finish" );
				while ( it->second.is_processing ) {
					m_mt_states_mutex.unlock();
					std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
					m_mt_states_mutex.lock();
				}
			}
			//Log( "MT Request " + to_string( mt_id ) + " canceled" );
			DestroyRequest( it->second.request );
			DestroyResponse( it->second.response );
			m_mt_states.erase( it );
		}
		m_mt_states_mutex.unlock();
	}

protected:

	// get request, return response
	virtual const RESPONSE_TYPE ProcessRequest( const REQUEST_TYPE& request, MT_CANCELABLE ) = 0;
	virtual void DestroyRequest( const REQUEST_TYPE& request ) = 0;
	virtual void DestroyResponse( const RESPONSE_TYPE& response ) = 0;

	void ProcessRequests() {
		mt_request_map_t requests = MT_GetRequests();
		if ( !requests.empty() ) {
			//Log( "MT Processing " + to_string( requests.size() ) + " requests" );
			mt_response_map_t responses = {};
			for ( auto& request : requests ) {
				m_mt_states_mutex.lock();
				const mt_id_t previous_request_id = m_current_request_id;
				const bool was_canceled = m_is_canceled;
				m_current_request_id = request.first;
				m_is_canceled = false;
				m_mt_states_mutex.unlock();
				responses[ request.first ] = ProcessRequest( request.second, m_is_canceled );
				m_mt_states_mutex.lock();
				m_current_request_id = previous_request_id;
				m_is_canceled = was_canceled;
				m_mt_states_mutex.unlock();
			}
			if ( !responses.empty() ) {
				MT_SetResponses( responses );
			}
		}
	}

private:

	struct mt_state_t {
		REQUEST_TYPE request = {};
		bool is_processing = false;
		bool is_executed = false;
		RESPONSE_TYPE response = {};
	};
	typedef std::map< mt_id_t, REQUEST_TYPE > mt_request_map_t;
	typedef std::map< mt_id_t, RESPONSE_TYPE > mt_response_map_t;

	mt_request_map_t MT_GetRequests() {
		mt_request_map_t result = {};
		m_mt_states_mutex.lock();
		for ( auto& it : m_mt_states ) {
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
			for ( auto& response : responses ) {
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

	typedef std::map< mt_id_t, mt_state_t > mt_states_t;
	std::mutex m_mt_states_mutex;
	mt_states_t m_mt_states = {};
	mt_flag_t m_is_canceled = false;
	std::atomic< mt_id_t > m_current_request_id = 0;
};

}
