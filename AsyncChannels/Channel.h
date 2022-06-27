#pragma once
#include <vector>
#include <memory>
#include <string>
#include "BaseChannel.h"
#include <functional>
#include <iostream>
#include <list>
#include "Event.h"

template<typename T>
class Channel : public BaseChannel
{
public:
	std::list<T> queue;
	std::list<std::function<void()>> waitingForRoom;
	using Receiver = std::function<bool(T&)>;
	using ReceiverKey = typename std::list<Receiver>::iterator;
	std::list<Receiver> receivers;
	Event<> onDestructEvent;
	const size_t maxSize = 0;

	Channel() = default;
	Channel(size_t maxSize) : maxSize(maxSize) {};
	~Channel() {
		BaseChannel::~BaseChannel();
		onDestructEvent();
	};

	template<typename Func>
	ReceiverKey addReceiver(Func&& func) {
		receivers.push_back(func);
		consume();
		return	--receivers.end();
	}

	void removeReceiver(ReceiverKey it) {
		receivers.erase(it);
	}

	void add_a_waitingItemToQueue() {
		if (waitingForRoom.size() > 0) {
			waitingForRoom.front()();
			waitingForRoom.pop_front();
		}
	}

	void consume() {
		while (queue.size() > 0) {
			auto front = queue.front();

			//consume if atleast 1 reciever consumed the item
			bool shouldConsume = false;
			for (auto& receiver : receivers) {
				auto consumed = receiver(front);
				if (consumed) {
					shouldConsume = true;
				}
			}

			if (shouldConsume) {
				queue.pop_front();
				add_a_waitingItemToQueue();
			}
			else {
				//stop consuming prematurely
				return;
			}
		}
	}

	bool send(T t) {
		if (maxSize == 0 || queue.size() < maxSize) {
			queue.push_back(t);
			consume();
			return true;
		}
		return false;
	}

	template<typename Func>
	void send(T t, Func&& func) {
		if (maxSize == 0 || queue.size() < maxSize) {
			queue.push_back(t);
			consume();
			func();
		}
		else {
			waitingForRoom.push_back([=]() {
				queue.push_back(t);
				consume();
				func();
			});
		}
	}

	//This function will be called when there is room to push the channel
	//return the value to be added to the Channel
	//Func signartion: T()
	template<typename Func>
	void async_send(Func&& func) {
		if (maxSize == 0 || queue.size() < maxSize) {
			queue.push_back(func());
			consume();
		} else {
			waitingForRoom.push_back([=]() {
				queue.push_back(func());
				consume();
			});
		}
	}
};

