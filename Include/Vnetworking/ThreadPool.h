/*
	Vnetworking Library
	Copyright (C) V0idPointer
*/

#ifndef _NE_THREADPOOL_H_
#define _NE_THREADPOOL_H_

#include <cstdint>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <tuple>
#include <exception>
#include <stdexcept>

namespace Vnetworking {

	template <typename... Ts>
	class ThreadPool {

	private:
		bool m_active;
		std::int32_t m_threadCount;
		std::vector<std::thread> m_threads;
		std::queue<std::tuple<std::function<void(Ts...)>, std::tuple<Ts...>>> m_jobQueue;

		mutable std::mutex m_mutex;
		std::condition_variable m_condition;

	public:
		ThreadPool(void);
		ThreadPool(const std::int32_t threadCount);
		ThreadPool(const ThreadPool&) = delete;
		ThreadPool(ThreadPool&&) noexcept = delete;
		virtual ~ThreadPool(void);

		ThreadPool& operator= (const ThreadPool&) = delete;
		ThreadPool& operator= (ThreadPool&&) noexcept = delete;

	private:
		void WorkerThreadProc(void);

	public:
		void EnqueueJob(const std::function<void(Ts...)> fn, Ts... args);
		std::int32_t GetThreadCount(void) const;
		std::int32_t GetJobCount(void) const;

	};

	template <typename... Ts>
	inline ThreadPool<Ts...>::ThreadPool() : ThreadPool<Ts...>(std::thread::hardware_concurrency()) { }

	template <typename... Ts>
	inline ThreadPool<Ts...>::ThreadPool(const std::int32_t threadCount) {

		if (threadCount < 1)
			throw std::invalid_argument("Cannot create a thread pool with zero or less threads.");

		this->m_active = true;
		this->m_threadCount = threadCount;
		this->m_threads.resize(threadCount);

		for (std::int32_t i = 0; i < this->m_threadCount; ++i)
			this->m_threads[i] = std::thread(&ThreadPool<Ts...>::WorkerThreadProc, this);

	}

	template <typename... Ts>
	inline ThreadPool<Ts...>::~ThreadPool() {

		this->m_active = false;
		this->m_condition.notify_all();

		for (std::int32_t i = 0; i < this->m_threadCount; ++i)
			this->m_threads[i].join();

	}

	template <typename... Ts>
	inline void ThreadPool<Ts...>::WorkerThreadProc() {
		while (true) {

			std::tuple<std::function<void(Ts...)>, std::tuple<Ts...>> t;
			{

				std::unique_lock<std::mutex> lock(this->m_mutex);
				this->m_condition.wait(lock, [&] (void) -> bool {
					return (!m_jobQueue.empty() || !m_active);
				});

				if (!this->m_active) return;

				t = this->m_jobQueue.front();
				this->m_jobQueue.pop();

			}

			std::apply(std::get<0>(t), std::get<1>(t));

		}
	}

	template <typename... Ts>
	inline void ThreadPool<Ts...>::EnqueueJob(const std::function<void(Ts...)> fn, Ts... args) {
		const std::lock_guard<std::mutex> lock(this->m_mutex);
		this->m_jobQueue.push({ fn, { args... } });
		this->m_condition.notify_one();
	}

	template <typename... Ts>
	inline std::int32_t ThreadPool<Ts...>::GetThreadCount() const {
		const std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_threadCount;
	}

	template <typename... Ts>
	inline std::int32_t ThreadPool<Ts...>::GetJobCount() const {
		const std::lock_guard<std::mutex> lock(this->m_mutex);
		return this->m_jobQueue.size();
	}

}

#endif // _NE_THREADPOOL_H_