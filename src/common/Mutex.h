#pragma once

#include <shared_mutex>

namespace common {

enum mutex_mode_t {
	MM_UNKNOWN,
	MM_NONE,
	MM_DEFAULT,
};

class Mutex {
public:

	static void SetMutexMode( const mutex_mode_t mutex_mode );

	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	std::shared_mutex* m_mutex = nullptr;

};

}
