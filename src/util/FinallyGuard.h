#pragma once

#include <functional>

namespace util {

struct FinallyGuard {
	std::function< void() > f_after;
	FinallyGuard( std::function< void() > after )
		: f_after( std::move( after ) ) {}
	~FinallyGuard() { if ( f_after ) { f_after(); } }
};

}
