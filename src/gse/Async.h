#pragma once

#include <map>
#include <vector>
#include <atomic>

#include "common/Common.h"
#include "gc/Object.h"

#include "Value.h"
#include "Types.h"

namespace gse {

namespace context {
class Context;
}

CLASS2( Async, common::Class, gc::Object )

	Async( gc::Space* const gc_space );

	typedef size_t timer_id_t;

	void Iterate( ExecutionPointer& ep );

	const timer_id_t StartTimer( const size_t ms, gse::Value* const f, GSE_CALLABLE_NOGC );
	const bool StopTimer( const timer_id_t id );
	void StopTimers();

	void ProcessAndExit( ExecutionPointer& ep );

	void GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) override;

private:

	gc::Space* const m_gc_space = nullptr;
	std::atomic< bool > m_is_stopping = false;
	std::mutex m_process_timers_mutex;

	struct timer_t {
		size_t ms;
		gse::Value* callable;
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
