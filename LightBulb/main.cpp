#include <System.h>
#include <boost/asio.hpp>
#include <iostream>
#include <Channel.h>
#include "RemoteChannels.h"
#include <list>
using boost::asio::ip::tcp;


std::unordered_map<size_t, BaseChannel> channels;
boost::asio::io_service io_service;


void readLightState(Channel<bool>& lightState) {
	lightState.addReceiver([&](auto f) {
		std::cout << f << std::endl;
		return true;
	});
}


std::thread readLightStateAsync(AsyncChannel<bool>& lightState) {
	return std::thread([&]() {
		std::cout << lightState.consume() << std::endl;
	});
}


void readLightTemp(Channel<float>& lightTemp) {
	auto temp = rand() / (float)RAND_MAX * 200;
	std::cout << "sending light temperature" << std::endl;
	lightTemp.send(temp);

	auto delay = boost::posix_time::seconds(1);
	static boost::asio::deadline_timer t(io_service, delay);

	t.expires_at(t.expires_at() + delay);
	t.async_wait([&](auto& error) {
		readLightTemp(lightTemp);
	});
}




void main(int argc, char** argv) {

	std::cout << argv[1] << " " << argv[2] << std::endl;
	std::cout << "Light Bulb" << std::endl;
	std::string name = "Light Bulb";

	RemoteChannels remote(io_service, std::stoi(argv[1]));


	auto lightState = std::make_shared<AsyncChannel<bool>>();
	Channel<float> lightTemp;


	//remote.subscribeChannelForRead("lightState", std::stoi(argv[2]), lightState);
	auto con = remote.subscribeChannelForWriteAsync("lightState", std::stoi(argv[2]), lightState);
	//remote.readIn("lightState", lightState);
	//remote.sendOut("lightTemp", lightTemp);

	auto thr = readLightStateAsync(*lightState);
	//readLightTemp(lightTemp);
	


	io_service.run();

	thr.join();
	con->join();
}


