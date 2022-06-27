#include "System.h"
#include <boost/asio.hpp>
#include <iostream>
#include "Channel.h"
#include "tcp_server.h"
#include "BroadcastWriter.h"
#include "BroadcastReader.h"

using boost::asio::ip::tcp;



	
//
//void main(int argc, char** argv) {
//
//	auto myPort = std::stoi(argv[1]);
//	auto remotePort =       argv[2];
//
//	tcp_server server(io_service, myPort);
//	server.start_accept();
//
//
//	
//	auto connection = std::make_shared<message_connection>(io_service);
//	tcp::resolver resolver(io_service);
//	tcp::resolver::query query("127.0.0.1", remotePort);
//	auto endpoint = resolver.resolve(query);
//	tcp::resolver::iterator end;
//	boost::asio::async_connect(connection->socket(), endpoint, [](auto& error, auto& endpoint) {
//		if (!error) {
//			std::cout << error << std::endl;
//		}
//	});
//	
//	
//
//	BroadcastWriter broadcastSender;
//
//	BroadcastReader broadcastReader;
//
//	broadcastSender.connections.push_back(connection);
//	broadcastReader.connection = connection;
//
//
//	//const std::string sendAll = "Send All";
//	//const std::string receiveAll = "Send All";
//	////should request all channel names
//	//sender->writeMesssage(sendAll, 1);
//
//	////got a request to send all our channel names
//	//sender->addResolver<int>(sendAll, [=](auto e) {
//	//	sender->writeMesssage(receiveAll, broadcastSender.names.size());
//	//	for (auto& name : broadcastSender.names) {
//	//		sender->write(name);
//	//	}
//	//});
//
//	////got a request to send all our channel names
//	//sender->addResolver<int>(receiveAll, [=](auto numberOfNames) {
//	//	std::vector<std::string> names;
//	//	for (size_t i = 0; i < numberOfNames; i++){
//	//		names.push_back(sender->read<std::string>());
//	//	}
//
//	//});
//
//
//
//
//	
//	//should request all channel names
//	
//
//	
//
//	Channel<float> temps;
//	Channel<bool> isBoiling1;
//	Channel<bool> isBoiling2;
//
//	
//
//
//
//	std::cout << "Started" << std::endl;
//
//	dispatch([&]() {
//		broadcastReader("isBoiling", isBoiling2);
//		//send to all recievers that isBoiling is waiting
//		
//	});
//
//	dispatch([&]() {
//		broadcastSender("isBoiling", isBoiling1);
//	});	
//
//	dispatch([&]() {
//		readTemp(temps);
//	});
//
//	dispatch([&]() {
//		computeIsBoiling(temps, isBoiling1);
//	});
//
//	dispatch([&]() {
//		printBoiling(isBoiling2);
//	});
//
//
//
//	io_service.run();
//
//}


