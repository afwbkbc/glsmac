#include "MainThreadDispatch.h"
#include "Assert.h"
#ifdef __APPLE__
#include <pthread.h>
#endif

namespace common {

MainThreadDispatch* MainThreadDispatch::GetInstance() {
	static MainThreadDispatch instance;
	return &instance;
}

void MainThreadDispatch::DispatchVoid(std::function<void()> func) {
	std::lock_guard<std::mutex> lock(m_queue_mutex);
	m_queue.push(func);
	m_queue_condition.notify_one();
}

void MainThreadDispatch::ProcessQueue() {
#ifdef __APPLE__
	ASSERT(pthread_main_np() != 0, "ProcessQueue must be called from main thread");
#endif

	std::unique_lock<std::mutex> lock(m_queue_mutex);
	
	int processed_count = 0;
	size_t queue_size = m_queue.size();
	
	// Process all queued operations
	while (!m_queue.empty()) {
		auto func = m_queue.front();
		m_queue.pop();
		processed_count++;
		
		// Unlock while executing to allow new operations to be queued
		lock.unlock();
		
		try {
			// Check if func is valid before calling
			if (!func) {
				lock.lock();
				continue; // Skip null function
			}
			func();
		} catch (const std::exception& e) {
		} catch (...) {
		}
		lock.lock();
	}
}

}

