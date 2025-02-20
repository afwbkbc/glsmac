#include "Async.h"

#include <thread>
#include <chrono>

#include "gse/value/Callable.h"
#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/context/ChildContext.h"
#include "Exception.h"

namespace gse {

// just a precaution
#define MAX_SLEEP_MS ( 1000 * 3600 )

Async::Async( gc::Space* const gc_space )
	: m_gc_space( gc_space ) {}

void Async::Iterate( ExecutionPointer& ep ) {
	while ( !m_timers.empty() && Now() >= m_timers.begin()->first ) {
		ProcessTimers( m_timers.begin(), ep );
	}
}

static Async::timer_id_t s_next_id = 0;

const Async::timer_id_t Async::StartTimer( const size_t ms, Value* const f, GSE_CALLABLE_NOGC ) {
	ASSERT( f->type == Value::T_CALLABLE, "invalid callable type: " + f->GetTypeString() );
	if ( s_next_id == SIZE_MAX - 1 ) {
		s_next_id = 0;
	}
	s_next_id++;
	auto* subctx = ctx->ForkAndExecute(
		m_gc_space, GSE_CALL_NOGC, true, [ this, &ms, &si, &ep, &f ]( gse::context::ChildContext* const subctx ) {
			ValidateMs( ms, m_gc_space, subctx, si, ep );
			subctx->PersistValue( f );
		}
	);
	const auto time = Now() + ms;
	m_timers[ time ].insert(
		{
			s_next_id,
			{
				ms,
				f,
				subctx,
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
	return s_next_id;
}

const bool Async::StopTimer( const gse::Async::timer_id_t id ) {
	const auto& it = m_timers_ms.find( id );
	if ( it == m_timers_ms.end() ) {
		return false;
	}
	const auto& time = it->second;
	ASSERT( m_timers.find( time ) != m_timers.end(), "timers not found" );
	auto& timers = m_timers.at( time );
	ASSERT( timers.find( id ) != timers.end(), "timer not found" );
	const auto& timer = timers.at( id );
	timer.ctx->UnpersistValue( timer.callable );
	timers.erase( id );
	if ( timers.empty() ) {
		m_timers.erase( time );
	}
	m_timers_ms.erase( id );
	return true;
}

void Async::StopTimers() {
	for ( const auto& it : m_timers ) {
		for ( const auto& it2 : it.second ) {
			it2.second.ctx->UnpersistValue( it2.second.callable );
		}
	}
	m_timers.clear();
	m_timers_ms.clear();
}

void Async::ProcessAndExit( ExecutionPointer& ep ) {
	while ( !m_timers.empty() ) {
		const auto& it = m_timers.begin();
		const auto now = Now();
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

const uint64_t Async::Now() const {
	return std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

void Async::ValidateMs( const int64_t ms, GSE_CALLABLE ) const {
	if ( ms < 0 ) {
		GSE_ERROR( EC.OPERATION_FAILED, "Timeout can't be negative: " + std::to_string( ms ) );
	}
	if ( ms > MAX_SLEEP_MS ) {
		GSE_ERROR( EC.OPERATION_FAILED, "Timeout can't be larger than " + std::to_string( MAX_SLEEP_MS ) + "ms: " + std::to_string( ms ) );
	}
}

void Async::ProcessTimers( const timers_t::iterator& it, ExecutionPointer& ep ) {
	std::map< uint64_t, std::map< timer_id_t, timer_t > > timers_new = {};

	for ( const auto& it2 : it->second ) {
		const auto& timer = it2.second;

		auto* ctx = timer.ctx;
		const auto f = timer.callable;
		const auto si = timer.si;
		const auto result = ( (value::Callable*)f )->Run( m_gc_space, GSE_CALL_NOGC, {} );

		size_t ms = 0;
		bool repeat = false;

		const auto& r = result;
		switch ( r->type ) {
			case Value::T_NOTHING:
			case Value::T_UNDEFINED:
			case Value::T_NULL:
				break;
			case Value::T_BOOL: {
				if ( ( (value::Bool*)r )->value ) {
					repeat = true;
					ms = timer.ms;
				}
				break;
			}
			case Value::T_INT: {
				ms = ( (value::Int*)r )->value;
				ValidateMs( ms, m_gc_space, GSE_CALL_NOGC );
				repeat = true;
				break;
			}
			default:
				GSE_ERROR( EC.INVALID_HANDLER, "Unexpected async return type. Expected: Nothing, Undefined, Null, Bool or Int,, got: " + result->GetTypeString() );
		}

		if ( repeat ) {
			timers_new[ Now() + ms ].insert(
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
		else {
			ctx->UnpersistValue( f );
		}
		ASSERT( m_timers_ms.find( it2.first ) != m_timers_ms.end(), "related timers_ms entry not found" );
		m_timers_ms.erase( it2.first );
	}

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
