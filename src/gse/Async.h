#pragma once

#include <map>
#include <vector>
#include <atomic>

#include "gc/Object.h"

#include "Value.h"
#include "Types.h"

namespace gse {

namespace value {
class Callable;
}

namespace context {
class Context;
}

CLASS( Async, gc::Object )

	Async( gc::Space* const gc_space );

	void Iterate( ExecutionPointer& ep );

	const timer_id_t StartTimer( const size_t ms, gse::value::Callable* const f, GSE_CALLABLE_NOGC );
	const bool StopTimer( const timer_id_t id );
	void StopTimers();

	gse::Value* const CreateTimer( const size_t ms, gse::value::Callable* const f, GSE_CALLABLE_NOGC, timer_id_t* const out_timer_id = nullptr );

	void ProcessAndExit( ExecutionPointer& ep );

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;

private:

	gc::Space* const m_gc_space = nullptr;
	std::atomic< bool > m_is_stopping = false;
	std::mutex m_process_timers_mutex;

	struct timer_t {
		size_t ms;
		gse::value::Callable* callable;
		gse::context::Context* ctx;
		gse::si_t si;
	};

	typedef std::map< uint64_t, std::map< timer_id_t, timer_t > > timers_t;
	timers_t m_timers = {};
	std::map< timer_id_t, uint64_t > m_timers_ms = {};

	void ValidateMs( const int64_t ms, GSE_CALLABLE ) const;
	void ProcessTimers( const timers_t::const_iterator& it, ExecutionPointer& ep );
};

}
