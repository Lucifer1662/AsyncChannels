#pragma once
#include "message_connection.h"
#include "BroadcastReader.h"
#include "BroadcastWriter.h";
#include "tcp_server.h"
#include "message_reciever.h"
#include "Channel_connection.h"
#include "message_receiver_direct.h"
#include "remote_channel.h"

using boost::asio::ip::tcp;

class RemoteChannels
{
	boost::asio::io_service& io_service;
	tcp_server server;
public:
	std::unordered_map<std::string, std::function<void(std::shared_ptr<message_connection>)>> addChannelConnection;


	RemoteChannels(boost::asio::io_service& io_service, int myPort) :
		io_service(io_service),
		server(io_service, myPort, [&](auto newConnection) {
		
		newConnection->addResolver<size_t>("bridge", [&, newConnection](auto channelNameSize) {
			std::string channelName(channelNameSize, ' ');
			newConnection->read((void*)channelName.c_str(), channelNameSize);
			auto it = addChannelConnection.find(channelName);
			if (it != addChannelConnection.end()) {
				addChannelConnection[channelName](newConnection);
			}
		});

		newConnection->readMessage();

	})
	{
		server.start_accept();
	}
	



	template<typename T>
	void publishChannelForRead(const std::string name, 
		std::shared_ptr<Channel<T>> channel = std::make_shared<Channel<T>>()) {

		addChannelConnection[name] = [=](auto connection) {
			channelToConnection(name, channel, connection);
			connection->start_reading();
		};
	}

	template<typename T>
	void subscribeChannelForRead(const std::string name, int port,
		std::shared_ptr<Channel<T>> channel = std::make_shared<Channel<T>>()) {

		auto connection = std::make_shared<message_reciever_direct>(io_service, name);
		//connect to the endpoint
		connection->connect("127.0.0.1", port);

		connectionToChannel(name, channel, connection);
	}


	template<typename T>
	void publishChannelForWrite(const std::string name,
		std::shared_ptr<Channel<T>> channel) {

		addChannelConnection[name] = [=](auto connection) {
			connectionToChannel(name, channel, connection);
			connection->start_reading();
		};
	}

	template<typename T>
	void subscribeChannelForWrite(const std::string& name, int port,
		std::shared_ptr<Channel<T>> channel) {

		auto connection = std::make_shared<message_reciever_direct>(io_service, name);
		//connect to the endpoint
		connection->connect("127.0.0.1", port);

		channelToConnection(name, channel, connection);
	}




	//ASYNC


	template<typename T>
	auto publishChannelForReadAsync(const std::string name,
		std::shared_ptr<AsyncChannel<T>> channel = std::make_shared<AsyncChannel<T>>()) {
		auto bridge = std::make_shared<remote_channel_reader<T>>();
		bridge->setChannel(channel);
		addChannelConnection[name] = [=](auto connection) {
			bridge->setConnection(connection);
		};
		return bridge;
	}

	template<typename T>
	auto subscribeChannelForWriteAsync(const std::string name, int port,
		std::shared_ptr<AsyncChannel<T>> channel = std::make_shared<AsyncChannel<T>>()) {

		auto connection = std::make_shared<message_reciever_direct>(io_service, name);

		//connect to the endpoint
		connection->connect("127.0.0.1", port);
		//connection->write(name);
		auto remote = std::make_shared<remote_channel_writer<T>>();
		remote->setConnection(connection);
		remote->setChannel(channel);
		return remote;
	}



};

