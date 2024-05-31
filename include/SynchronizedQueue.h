#pragma once

#include <queue>
#include <mutex>

template<typename Type>
class SynchronizedQueue {
protected:
	std::queue<Type> queue;
	std::mutex queueAccessMutex;
public:
	void insert(Type obj) {
		std::lock_guard lock(queueAccessMutex);
		queue.push(std::move(obj));
	}

	std::optional<Type> pop() {
		std::lock_guard lock(queueAccessMutex);

		if (queue.empty()) {
			return std::nullopt;
		}

		auto result = queue.front();
		queue.pop();
		return result;
	}
};