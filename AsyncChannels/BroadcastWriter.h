#pragma once
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
#include "Channel.h"
#include "tcp_connection.h"
#include "message_connection.h"
#include <unordered_map>


class BroadcastWriter {
public:
	std::unordered_map<endpoint, std::shared_ptr<message_connection>> connections;


	template<typename T>
	void consumeThenSend(std::string name, Channel<T>& outChannel) {
		outChannel.addReceiver([&,name](auto isBoiling) {
			/*for (auto& connection : connections) {
				connection->writeMesssage(name, isBoiling);
			}*/
			return true;
		});
	}

	template<typename T>
	void sendThenConsume(std::string name, Channel<T>& outChannel, endpoint _endpoint) {
		
		auto it = connections.find(_endpoint);
		if(it)

		outChannel.addReceiver([&, name](auto item) {
			bool beenRecieved = false;
			for (auto& connection : connections) {
				auto suc = connection->writeMesssage(name, item);
				if (suc) {
					beenRecieved = true;
				}
			}
			return beenRecieved;
		});
	}

	void addConnection(std::shared_ptr<message_connection> connection) {
		connections.insert_or_assign(connection->socket().remote_endpoint(), connection);
		connection->onErrorEvent.push_back([](auto error) {
				
		});
	}
};



