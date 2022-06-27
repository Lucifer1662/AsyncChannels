#pragma once
#include <iostream>
#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using error_code = boost::system::error_code;
using endpoint = boost::asio::ip::tcp::endpoint;

#include "Event.h"

class tcp_connection
{
	tcp::iostream stream_;
	std::weak_ptr<tcp_connection> self;

	std::string host;
	int port;


public:
	Event<error_code> onErrorEvent;
	Event<error_code&, endpoint&> onConnectEvent;
	Event<error_code&, endpoint&> onReconnectEvent;
	Event<> onDestructEvent;
	boost::asio::io_service& io_service;


	template<typename T>
	T read() {
		T t;
		auto buffer = boost::asio::buffer((void*)&t, sizeof(T));
		boost::asio::read(socket(), buffer, boost::asio::transfer_exactly(sizeof(T)));
		return t;
	}

	template<>
	std::string read<std::string>() {
		auto size = read<size_t>();
		std::string t(size, ' ');
		read((void*)t.c_str(), sizeof(t));
		return t;
	}



	template<typename Handler>
	void async_wait_for_read(Handler&& handler) {
		socket().async_wait(boost::asio::ip::tcp::socket::wait_read, handler);
	}

	template<typename Handler>
	void async_wait_for_error(Handler&& handler) {
		socket().async_wait(boost::asio::ip::tcp::socket::wait_error, handler);
	}
	

	template<typename T, typename Handler>
	void async_read(Handler&& handler) {
		auto t = std::make_shared<T>();
		auto buffer = boost::asio::buffer((void*)&(*t), sizeof(T));
		boost::asio::async_read(socket(), buffer, boost::asio::transfer_exactly(sizeof(T)), 
			[=](auto error) {
			handler(*t);
		});
	}

	void read(void* data, int size);

	bool write(void* data, int size) {
		boost::system::error_code ec;
		try {
			boost::asio::write(socket(), boost::asio::buffer(data, size), ec);
		}
		catch (std::exception e) {
			onError(ec);
		}
		return !ec.failed();
	}

	bool writeBuf(boost::asio::streambuf& buffer) {
		error_code ec;
		try {
			boost::asio::write(socket(), buffer, ec);
		}
		catch (std::exception e) {
			onError(ec);
		}
		return !ec.failed();
	}

	template<typename T>
	bool write(T object) {
		boost::system::error_code ec;
		try {
			boost::asio::write(socket(), boost::asio::buffer((void*)&object, sizeof(T)), ec);
		}
		catch (std::exception e) {
			onError(ec);
		}
		return !ec.failed();;
	}

	template<>
	bool write<std::string>(std::string t) {
		auto suc1 = tcp_connection::write<size_t>(t.size());
		auto suc2 = tcp_connection::write((void*)t.c_str(), t.size());
		return suc1 && suc2;
	}

	typedef std::shared_ptr<tcp_connection> pointer;
	static pointer create(boost::asio::io_service& io_service);

	tcp::socket& socket();

	void start();

	tcp::iostream& stream();

	//tcp_connection(const tcp_connection&) = default;
	//tcp_connection(tcp_connection&&) = default;
	tcp_connection(boost::asio::io_service& io_service);

	template<typename Handler>
	void connect(Handler&& handler) {
		tcp::resolver resolver(io_service);
		tcp::resolver::query query("127.0.0.1", std::to_string(port));
		auto endpoint = resolver.resolve(query);
		tcp::resolver::iterator end;
		boost::asio::async_connect(socket(), endpoint, handler);
	}

	void connect(std::string host, int port) {
		this->host = host;
		this->port = port;
		connect([&](auto error, auto endpoint) {
			this->onConnect(error, endpoint);
			std::cout << endpoint.port() << std::endl;
		});
		
	}

	virtual void onConnect(error_code& error, endpoint& endpoint) {
		if (error)
			onError(error);
		else
			onConnectEvent(error, endpoint);
	}

	void reconnect() {
		connect([&](auto error, auto endpoint) {
			this->onReconnect(error, endpoint);
		});
	}

	virtual void onReconnect(error_code& error, endpoint& endpoint) {
		if (error)
			onError(error);
		else
			onReconnectEvent(error, endpoint);
	}

	virtual void onError(error_code& error) {
		onErrorEvent(error);
	}

	void start_reading() {
		async_wait_for_read([=](auto error) {
			this->onRead(error);
		});
	}

	virtual void onRead(error_code& error) {
		if (error)
			onError(error);
	}

	~tcp_connection() {
		onDestructEvent();
	}
};