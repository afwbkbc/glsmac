#include "Time.h"

#include <chrono>

namespace util {

const uint64_t Time::Now() {
	return std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::system_clock::now().time_since_epoch()
	).count();
}

}
