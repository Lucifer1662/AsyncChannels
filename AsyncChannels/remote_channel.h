#pragma once
#include "tcp_connection.h"
#include <functional>
#include "AsyncChannel.h"
#include <memory>
#include <thread>


template<typename T>
class remote_channel
{
	std::unique_ptr<std::thread> thread;

public:
	std::shared_ptr<tcp_connection> connection;
	std::shared_ptr<AsyncChannel<T>> channel;

	void setConnection(std::shared_ptr<tcp_connection> connection) {
		this->connection = connection;
		if (connection && channel)
			thread = std::make_unique<std::thread>([&]() {this->run(); });
	}

	void setChannel(std::shared_ptr<AsyncChannel<T>> channel) {
		this->channel = channel;
		if (connection && channel)
			thread = std::make_unique<std::thread>([&]() {this->run(); });
	}

	virtual void run() {
		int w = 0;
	}

	void join() { thread->join(); }
};


template<typename T>
class remote_channel_writer : public remote_channel<T>
{
	virtual void run() {
		while (true) {
			auto value = this->channel->consume();
			this->connection->write(value);
		}
	}

};

template<typename T>
class remote_channel_reader : public remote_channel<T>
{
	void readMessage() {
		//read in from connection
		auto value = this->connection->read<T>();
		//send to buffer
		this->channel->send(value);
		//once buffer has accepted it send back success
		size_t success = 1;
		this->connection->write(success);
	}

	virtual void run() {
	/*	this->connection->read();*/

		while (true) {
			readMessage();
		}
	}

};

