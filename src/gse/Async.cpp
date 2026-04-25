#include "Async.h"

#include <thread>
#include <chrono>

#include "gse/value/Callable.h"
#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/context/ChildContext.h"
#include "Exception.h"
#include "util/Time.h"
#include "gc/Space.h"
#include "gse/callable/Native.h"
#include "gse/value/Undefined.h"
#include "gse/value/Object.h"

namespace gse {

// just a precaution
#define MAX_SLEEP_MS ( 1000 * 3600 )

Async::Async( gc::Space* const gc_space )
	: gc::Object( gc_space )
	, m_gc_space( gc_space ) {}

void Async::Iterate( ExecutionPointer& ep ) {
	if ( !m_is_stopping ) {
		while ( !m_timers.empty() && util::Time::Now() >= m_timers.begin()->first ) {
			ProcessTimers( m_timers.begin(), ep );
		}
	}
}

static timer_id_t s_next_id = 0;

const timer_id_t Async::StartTimer( const size_t ms, gse::value::Callable* const f, GSE_CALLABLE_NOGC ) {
	ASSERT( f->type == VT_CALLABLE, "invalid callable type: " + f->GetTypeString() );
	if ( s_next_id == SIZE_MAX - 1 ) {
		s_next_id = 0;
	}
	s_next_id++;
	{
		ValidateMs( ms, m_gc_space, ctx, si, ep );
		const auto time = util::Time::Now() + ms;
		m_timers[ time ].insert(
			{
				s_next_id,
				{
					ms,
					f,
					ctx,
					si
				}
			}
		);
		m_timers_ms.insert(
			{
				s_next_id,
				time
			}
		);
	}
	return s_next_id;
}

const bool Async::StopTimer( const gse::timer_id_t id ) {
	const auto& it = m_timers_ms.find( id );
	if ( it == m_timers_ms.end() ) {
		return false;
	}
	const auto& time = it->second;
	ASSERT( m_timers.find( time ) != m_timers.end(), "timers not found" );
	auto& timers = m_timers.at( time );
	ASSERT( timers.find( id ) != timers.end(), "timer not found" );
	timers.erase( id );
	if ( timers.empty() ) {
		m_timers.erase( time );
	}
	m_timers_ms.erase( id );
	return true;
}

void Async::StopTimers() {
	m_timers.clear();
	m_timers_ms.clear();
}

gse::Value* const Async::CreateTimer( const size_t ms, gse::value::Callable* const f, GSE_CALLABLE_NOGC, timer_id_t* const out_timer_id ) {
	const auto timer_id = StartTimer( ms, f, GSE_CALL_NOGC );

	auto* const gc_space = m_gc_space;
	const gse::value::object_properties_t properties = {
		{
			"stop",
			// recursive NATIVE_CALL doesn't work
			NATIVE_CALL( this, timer_id ) {
				N_EXPECT_ARGS( 0 );
				StopTimer( timer_id );
				return VALUE( gse::value::Undefined );
			} ),
		}
	};

	if ( out_timer_id ) {
		*out_timer_id = timer_id;
	}

	return VALUEEXT( gse::value::Object, GSE_CALL, properties, "async" );
}

void Async::ProcessAndExit( ExecutionPointer& ep ) {
	m_is_stopping = true;
	m_process_timers_mutex.lock(); // wait for anything processing timers to finish
	m_process_timers_mutex.unlock();
	{
		while ( !m_timers.empty() ) {
			const auto& it = m_timers.begin();
			const auto now = util::Time::Now();
			const auto sleep_for = it->first > now
				? it->first - now
				: 0;

			//Log( "Waiting for " + std::to_string( sleep_for ) + "ms" );
			if ( sleep_for > 0 ) {
				std::this_thread::sleep_for( std::chrono::milliseconds( sleep_for ) );
			}
			ProcessTimers( it, ep );
		}
	}
}

void Async::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Async" );

	// timer callables are reachable
	GC_DEBUG_BEGIN( "timer_callables" );
	for ( const auto& timers : m_timers ) {
		for ( const auto& timer : timers.second ) {
			GC_REACHABLE( timer.second.callable );
			ASSERT( reachable_objects.find( timer.second.ctx ) != reachable_objects.end(), "callable context not reachable" );
		}
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

void Async::ValidateMs( const int64_t ms, GSE_CALLABLE ) const {
	if ( ms < 0 ) {
		GSE_ERROR( EC.OPERATION_FAILED, "Timeout can't be negative: " + std::to_string( ms ) );
	}
	if ( ms > MAX_SLEEP_MS ) {
		GSE_ERROR( EC.OPERATION_FAILED, "Timeout can't be larger than " + std::to_string( MAX_SLEEP_MS ) + "ms: " + std::to_string( ms ) );
	}
}

void Async::ProcessTimers( const timers_t::const_iterator& it, ExecutionPointer& ep ) {
	std::lock_guard guard( m_process_timers_mutex );

	std::map< uint64_t, std::map< timer_id_t, timer_t > > timers_new = {};

	const auto timers = it->second;

	for ( const auto& it2 : timers ) {
		const auto& timer = it2.second;

		auto* ctx = timer.ctx;
		const auto f = timer.callable;
		const auto si = timer.si;

		size_t ms = 0;
		bool repeat = false;
		m_gc_space->Accumulate(
			this,
			[ this, &ctx, &ep, &si, &f, &timer, &repeat, &ms ]() {

				const auto result = f->Run( m_gc_space, GSE_CALL_NOGC, {} );

				const auto& r = result;
				if ( r ) {
					switch ( r->type ) {
						case VT_UNDEFINED:
						case VT_NULL:
							break;
						case VT_BOOL: {
							if ( ( (value::Bool*)r )->value ) {
								repeat = true;
								ms = timer.ms;
							}
							break;
						}
						case VT_INT: {
							ms = ( (value::Int*)r )->value;
							ValidateMs( ms, m_gc_space, GSE_CALL_NOGC );
							repeat = true;
							break;
						}
						default:
							GSE_ERROR( EC.INVALID_HANDLER, "Unexpected async return type. Expected: Nothing, Undefined, Null, Bool or Int,, got: " + result->GetTypeString() );
					}
				}
			}
		);

		if ( repeat ) {
			timers_new[ util::Time::Now() + ms ].insert(
				{
					it2.first,
					{
						ms,
						f,
						ctx,
						si,
					}
				}
			);
		}

		if ( m_timers_ms.find( it2.first ) == m_timers_ms.end() ) {
			// timer was deleted in its own handler, should restart the whole cycle
			return;
		}

		m_timers_ms.erase( it2.first );
	}

	{
		m_timers.erase( it );
		for ( const auto& it_new : timers_new ) {
			for ( const auto& timer : it_new.second ) {
				m_timers[ it_new.first ].insert( timer );
				m_timers_ms.insert(
					{
						timer.first,
						it_new.first
					}
				);
			}
		}
	}
}

}
