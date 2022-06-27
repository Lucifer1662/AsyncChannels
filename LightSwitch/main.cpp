#include <System.h>
#include <boost/asio.hpp>
#include <iostream>
#include <Channel.h>
#include "tcp_server.h"
#include "BroadcastWriter.h"
#include "BroadcastReader.h"
#include <RemoteChannels.h>
#include <AsyncChannel.h>

using boost::asio::ip::tcp;


std::unordered_map<size_t, BaseChannel> channels;
boost::asio::io_service io_service;
boost::asio::io_service async_io_service;



void lightSwitchFlipped(Channel<bool>& lightState) {
	/*auto delay = boost::posix_time::seconds(1);
	static boost::asio::deadline_timer t(io_service, delay);*/

	lightState.async_send([&]() {
	
		std::cout << "switched - " << lightState.receivers.size() << std::endl;


	/*	t.expires_at(t.expires_at() + delay);
		t.async_wait([&](auto& error) {
			lightSwitchFlipped(lightState);
		});*/

		//queue up the next, but only when done
		//lightSwitchFlipped(lightState);

		return rand() % 2 == 0;
	});
	
}


std::thread lightSwitchFlippedAsync(AsyncChannel<bool>& lightState) {

	return std::thread([&]() {
		while (true) {
			lightState.send(rand() % 2 == 0);
			std::cout << "switched - " << std::endl;

			Sleep(1000);
		}
	});
}

std::thread lightBulbFlippedAsync(AsyncChannel<bool>& lightState) {

	return std::thread([&]() {
		while (true) {
			auto state = lightState.consume();
			std::cout << state << std::endl;

			Sleep(2000);
		}
	});
}



void lightSwitchFlippedSync(Channel<bool>& lightState) {
	lightState.send(rand() % 2==0);
	std::cout << "switched - " << lightState.receivers.size() << std::endl;
	auto delay = boost::posix_time::seconds(1);
	static boost::asio::deadline_timer t(io_service, delay);
	
	t.expires_at(t.expires_at() + delay);
	t.async_wait([&](auto& error) {
		lightSwitchFlipped(lightState);
	});
}


void printLightTemp(Channel<float>& lightTemp) {
	lightTemp.addReceiver([](float temp) {
		std::cout << "the light bulb temp is " << temp << std::endl;
		return true;
	});
}



void main(int argc, char** argv) {
	
	std::cout << argv[1] << " " << argv[2] << std::endl;
	std::string name = "Light Switch";
	std::cout << "Light Switch" << std::endl;

	RemoteChannels remote(io_service, std::stoi(argv[1]));

	//create channels
	auto lightState = std::make_shared<Channel<bool>>(1);
	Channel<float> lightTemp;

	auto lightStateAsync = std::make_shared<AsyncChannel<bool>>(1);



	//remote.publishChannelForRead("lightState", lightState);

	//connect channels to remote devices
	//remote.readIn("lightState", *lightState);
	//remote.sendOut("lightTemp", lightTemp);

	//device logic
	auto thread1 = lightSwitchFlippedAsync(*lightStateAsync);
	//auto thread2 = lightBulbFlippedAsync(*lightStateAsync);

	auto bridge = remote.publishChannelForReadAsync("lightState", lightStateAsync);

	//lightSwitchFlipped(*lightState);
	//printLightTemp(lightTemp);

	io_service.run();
	thread1.join();
	bridge->join();
	//thread2.join();

}


