#pragma once

#include <map>
#include <vector>

#include "common/Common.h"

#include "Value.h"
#include "Types.h"

namespace gse {

namespace context {
class Context;
}

CLASS( Async, common::Class )

	void Iterate();

	void AddTimer( const size_t ms, const gse::Value& f, gse::context::Context* const ctx, const gse::si_t& si );
	void StopTimers();
	
	void ProcessAndExit();

private:

	struct timer_t {
		size_t ms;
		gse::Value callable;
		gse::context::Context* ctx;
		gse::si_t si;
	};

	typedef std::map< uint64_t, std::vector< timer_t > > timers_t;
	timers_t m_timers = {};

	const uint64_t Now() const;
	void ValidateMs( const int64_t ms, context::Context* ctx, const si_t& call_si ) const;
	void ProcessTimers( const timers_t::iterator& it );
};

}
