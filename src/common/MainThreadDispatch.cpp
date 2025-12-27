#include "MainThreadDispatch.h"
#include "Assert.h"
#ifdef __APPLE__
#include <pthread.h>
#endif
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

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
	// Process all queued operations
	while (!m_queue.empty()) {
		auto func = m_queue.front();
		m_queue.pop();
		processed_count++;
		
		// Unlock while executing to allow new operations to be queued
		lock.unlock();
		func();
		lock.lock();
	}
	
	// #region agent log
	if (processed_count > 0) {
		std::ofstream log("/Users/jhurliman/Documents/Code/jhurliman/glsmac/.cursor/debug.log", std::ios::app);
		auto tid = std::this_thread::get_id();
		std::stringstream ss;
		ss << tid;
		auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
#ifdef __APPLE__
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"dispatch-test\",\"hypothesisId\":\"J\",\"location\":\"MainThreadDispatch.cpp:ProcessQueue\",\"message\":\"Processed queued operations\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"isMainThread\":" << (pthread_main_np() != 0 ? "true" : "false") << ",\"processedCount\":" << processed_count << "},\"timestamp\":" << now << "}\n";
#else
		log << "{\"sessionId\":\"debug-session\",\"runId\":\"dispatch-test\",\"hypothesisId\":\"J\",\"location\":\"MainThreadDispatch.cpp:ProcessQueue\",\"message\":\"Processed queued operations\",\"data\":{\"threadId\":\"" << ss.str() << "\",\"processedCount\":" << processed_count << "},\"timestamp\":" << now << "}\n";
#endif
	}
	// #endregion
}

}

