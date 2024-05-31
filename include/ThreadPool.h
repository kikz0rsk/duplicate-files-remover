#pragma once

#include <vector>
#include <thread>
#include <functional>
#include "SynchronizedQueue.h"

template<typename T = std::function<void()>>
class ThreadPool {
protected:
	std::vector<std::shared_ptr<std::thread>> threads{};
	unsigned int numThreads;
	SynchronizedQueue<T> queue;
	std::atomic<bool> endFlag = false;

public:
	explicit ThreadPool(unsigned int numThreads) : numThreads(numThreads) {
		reset();
	}

	ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

	void queueTask(T task) {
		queue.insert(task);
	}

	void wait() {
		endFlag = true;
		for (auto &thread: threads) {
			thread->join();
		}
	}

	void reset() {
		threads.reserve(numThreads);
		for (int i = 0; i < numThreads; i++) {
			threads.emplace_back(std::make_shared<std::thread>([this]() {
				while (true) {
					std::optional<T> callable = queue.pop();
					if (!callable) {
						if (endFlag) {
							return;
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						continue;
					}

					(*callable)();
				}
			}));
		}
	}
};