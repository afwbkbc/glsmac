#include "Async.h"

#include <thread>
#include <chrono>

#include "gse/type/Callable.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/context/ChildContext.h"
#include "Exception.h"

namespace gse {

// just a precaution
#define MAX_SLEEP_MS ( 1000 * 3600 )

void Async::AddTimer( const size_t ms, const Value& f, context::Context* const ctx, const si_t& si ) {
	ASSERT( f.Get()->type == type::Type::T_CALLABLE, "invalid callable type: " + f.GetTypeString() );
	ctx->IncRefs();
	auto* subctx = ctx->ForkContext( ctx, si, true );
	ValidateMs( ms, subctx, si );
	subctx->IncRefs();
	subctx->PersistValue( f );
	m_timers[ Now() + ms ].push_back(
		{
			ms,
			f,
			subctx,
			si
		}
	);
}

void Async::ProcessAndExit() {
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

		std::map< uint64_t, std::vector< timer_t > > timers_new = {};

		for ( const auto& timer : it->second ) {

			auto* ctx = timer.ctx;
			const auto f = timer.callable;
			const auto call_si = timer.si;
			const auto result = ( (type::Callable*)f.Get() )->Run( ctx, call_si, {} );

			size_t ms = 0;
			bool repeat = false;

			const auto& r = result.Get();
			switch ( r->type ) {
				case type::Type::T_NOTHING:
				case type::Type::T_UNDEFINED:
				case type::Type::T_NULL:
					break;
				case type::Type::T_BOOL: {
					if ( ( (type::Bool*)r )->value ) {
						repeat = true;
						ms = timer.ms;
					}
					break;
				}
				case type::Type::T_INT: {
					ms = ( (type::Int*)r )->value;
					ValidateMs( ms, ctx, call_si );
					repeat = true;
					break;
				}
				default:
					GSE_ERROR( EC.INVALID_HANDLER, "Unexpected async return type. Expected: Nothing, Undefined, Null, Bool or Int, got: " + result.GetTypeString() );
			}

			if ( repeat ) {
				timers_new[ Now() + ms ].push_back(
					{
						ms,
						f,
						ctx,
						call_si
					}
				);
			}
			else {
				ctx->UnpersistValue( f );
				ctx->DecRefs();
			}
		}

		m_timers.erase( it );
		for ( const auto& it_new : timers_new ) {
			for ( const auto& timer : it_new.second ) {
				m_timers[ it_new.first ].push_back( timer );
			}
		}
	}
}

const uint64_t Async::Now() const {
	return std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

void Async::ValidateMs( const int64_t ms, context::Context* ctx, const si_t& call_si ) const {
	if ( ms < 0 ) {
		GSE_ERROR( EC.OPERATION_FAILED, "Timeout can't be negative: " + std::to_string( ms ) );
	}
	if ( ms > MAX_SLEEP_MS ) {
		GSE_ERROR( EC.OPERATION_FAILED, "Timeout can't be larger than " + std::to_string( MAX_SLEEP_MS ) + "ms: " + std::to_string( ms ) );
	}
}

}
