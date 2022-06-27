#pragma once
#include "Channel.h"
#include "message_connection.h"
#include <vector>
#include <string>

class BroadcastReader {
public:
	std::vector<std::shared_ptr<message_connection>> connections;


	template<typename T>
	void operator()(std::string name, Channel<T>& inChannel) {
		for (auto& connection : connections) {
			connection->addResolver<T>(name, [&](T t) {
				inChannel.send(t);
			});
		}
	}

	void addConnection(std::shared_ptr<message_connection> connection) {
		connections.push_back(connection);
	}
};

