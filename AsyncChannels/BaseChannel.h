#pragma once

struct BaseChannel {
	virtual void put(void* content) {};

	virtual void send() {};

	virtual ~BaseChannel() {}
};

