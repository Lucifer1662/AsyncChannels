#pragma once
#include "message_connection.h"

class message_reciever : public message_connection
{
public:
	message_reciever(const message_reciever&) = default;
	message_reciever(message_reciever&&) = default;
	message_reciever(boost::asio::io_service & io_service) :
		message_connection(io_service) {}


	void onConnect(error_code& error, endpoint& endpoint) override {
		message_connection::onConnect(error, endpoint);
		if (!error) {
			start_reading();
		}
	}

	void onReconnect(error_code& error, endpoint& endpoint) override {
		message_connection::onReconnect(error, endpoint);
		if (!error) {
			start_reading();
		}
	}

	void onError(error_code& error) override {
		message_connection::onError(error);
		std::cout << "error " << error << std::endl;
		retryConnectionIn(5);
	}

	void retryConnectionIn(int delaySeconds) {
		auto delay = boost::posix_time::seconds(delaySeconds);
		boost::asio::deadline_timer timer(io_service, delay);

		//when an error occurs set a timer 
		timer.expires_at(timer.expires_at() + delay);
		timer.async_wait([&](auto& error) {
			//after a set time, rety to connect to the connection
			reconnect();
		});
	}
};

