#pragma once
#include "message_connection.h"
#include "Channel.h"



template<typename T>
void connectionToChannel(std::string name, std::shared_ptr<Channel<T>> channel,
	std::shared_ptr<message_connection> connection) {

	connection->addResolver<T>(name, [=](auto data) {
		channel->send(data);
	});

	//force the channel to hold onto a shared_ptr to the connection, to keep it alive,
	//until the channel dies, also remove the name from the connection, incase the conenction is being still used
	channel->onDestructEvent.push_back([=]() { connection->removeResolver(name); });
}

template<typename T>
void channelToConnection(std::string name, std::shared_ptr<Channel<T>> channel,
	std::shared_ptr<message_connection> connection) {
	
	auto recieverKey = channel->addReceiver([=](auto data) {
		return connection->writeMesssage(name, data);
	});

	connection->onErrorEvent.push_to_single([=](auto error) {
		channel->removeReceiver(recieverKey);
	});


	connection->onReconnectEvent.push_back([=](auto error, auto endpoint) {
		auto recieverKey = channel->addReceiver([=](auto data) {
			return connection->writeMesssage(name, data);
		});

		//if an error occurs remove reciever
		connection->onErrorEvent.push_to_single([=](auto error) {
			channel->removeReceiver(recieverKey);
		});
	});
}
