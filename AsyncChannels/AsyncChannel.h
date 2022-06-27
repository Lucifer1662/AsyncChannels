#pragma once
#include <vector>
#include <memory>
#include <string>
#include "BaseChannel.h"
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <condition_variable>
#include "Event.h"

int i = 0;
template<typename T>
class AsyncChannel : public BaseChannel
{
public:
	std::list<T> queue;
	const size_t maxSize = 0;
	std::mutex m;
	std::condition_variable cv_send;
	std::condition_variable cv_consume;

	AsyncChannel() = default;
	AsyncChannel(size_t maxSize) : maxSize(maxSize) {};
	~AsyncChannel() {
		BaseChannel::~BaseChannel();
	};


	bool canConsume() {
		return queue.size() > 0;
	}

	T consume() {
		std::unique_lock<std::mutex> lk(m);
		if (!canConsume()) {
			cv_consume.wait(lk, [] {
				std::cout << "check :" << i << std::endl;
				return i > 1;
					//canConsume();
			});
		}
		auto front = queue.front();
		queue.pop_front();
		cv_send.notify_one();	

		//slk.unlock();
		return front;
	}

	template<typename Func>
	bool read(Func&& func) {
		std::unique_lock<std::mutex> lk(m);
		if (!canConsume()) {
			cv_consume.wait(lk, [&] {
				return canConsume();
			});
		}
		if (func(queue.front())) {
			queue.pop_front();
			cv_send.notify_one();
		}
		//lk.unlock();
	}

	bool canInsert() {
		return maxSize == 0 || queue.size() < maxSize;
	}

	void send(T t) {
		std::unique_lock<std::mutex> lk(m);
		if (!canInsert()) {
			cv_send.wait(lk, [&] {return canInsert(); });
		}
		queue.push_back(t);
		i++;
		std::cout << i << std::endl;
		//lk.unlock();
		cv_consume.notify_one();
	}
};

