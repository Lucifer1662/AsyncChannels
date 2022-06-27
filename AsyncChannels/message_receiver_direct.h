#pragma once
#include "message_reciever.h"

class message_reciever_direct : public message_reciever
{
public:
	message_reciever_direct(const message_reciever_direct&) = default;
	message_reciever_direct(message_reciever_direct&&) = default;
	message_reciever_direct(boost::asio::io_service& io_service, std::string channelName) :
		message_reciever(io_service), channelName(channelName) {}

	std::string channelName;

	void onConnect(error_code& error, endpoint& endpoint) override {
		this->writeMesssage("bridge", channelName.size());
		this->write((void*)channelName.c_str(), channelName.size());

		message_reciever::onConnect(error, endpoint);
	}

	void onReconnect(error_code& error, endpoint& endpoint) override {
		this->writeMesssage("bridge", channelName.size());
		this->write((void*)channelName.c_str(), channelName.size());

		message_reciever::onReconnect(error, endpoint);
	}
};

