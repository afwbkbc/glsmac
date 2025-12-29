#pragma once

#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include <memory>

namespace common {

class MainThreadDispatch {
public:
	static MainThreadDispatch* GetInstance();

	// Dispatch an operation to run on the main thread
	// Returns a future that will contain the result when the operation completes
	template<typename R>
	std::future<R> Dispatch(std::function<R()> func) {
		auto promise = std::make_shared<std::promise<R>>();
		auto future = promise->get_future();

		{
			std::lock_guard<std::mutex> lock(m_queue_mutex);
			m_queue.push([promise, func]() {
				try {
					promise->set_value(func());
				} catch (...) {
					promise->set_exception(std::current_exception());
				}
			});
		}

		m_queue_condition.notify_one();
		return future;
	}

	// Dispatch a void operation (no return value)
	void DispatchVoid(std::function<void()> func);

	// Process all queued operations (must be called from main thread)
	void ProcessQueue();

private:
	MainThreadDispatch() = default;
	~MainThreadDispatch() = default;
	MainThreadDispatch(const MainThreadDispatch&) = delete;
	MainThreadDispatch& operator=(const MainThreadDispatch&) = delete;

	std::mutex m_queue_mutex;
	std::condition_variable m_queue_condition;
	std::queue<std::function<void()>> m_queue;
};

}

