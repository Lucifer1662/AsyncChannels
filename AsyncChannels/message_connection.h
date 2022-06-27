#pragma once
#include "tcp_connection.h"
#include <unordered_map>
#include <functional>


class message_connection : public tcp_connection
{
	std::unordered_map<std::string, std::function<void()>> resolvers;

public:
	message_connection(const message_connection&) = default;
	message_connection(message_connection&&) = default;
	message_connection(boost::asio::io_service& io_service) :
		tcp_connection(io_service) {}

	template<typename T>
	bool writeMesssage(std::string name, T& data) {
		auto suc1 = tcp_connection::write(name);
		auto suc2 = tcp_connection::write(data);
		return suc1 && suc2;
	}

	template<typename T>
	bool writeMesssage(std::string name, T&& data) {
		auto suc1 = tcp_connection::write(name);
		auto suc2 = tcp_connection::write(data);
		return suc1 && suc2;
	}

	void readMessage() {
		auto size = tcp_connection::read<size_t>();
		std::string name = std::string(size, ' ');
		tcp_connection::read((char*)name.c_str(), size);
		resolvers[name]();
	}

	template<typename T, typename Func>
	void addResolver(std::string name, Func&& func) {
		resolvers.insert({ { name, [&, func]() {
			 func(tcp_connection::read<T>());
		} } });
	}

	void removeResolver(std::string name) {
		resolvers.erase(name);
	}


	void onRead(error_code& error) override {
		tcp_connection::onRead(error);
		if (!error) {
			readMessage();
			start_reading();
		}
		else {
			std::cout << "async read " << error << std::endl;
		}
	}

};

