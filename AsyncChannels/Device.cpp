#include "Device.h"
#include "Packet.h"
#include "Channel.h"

Packet_Id* recieve() {
	return nullptr;
}


static std::vector<Device*> devices;

Device::Device(std::string name) : name(name) { devices.push_back(this); }

void Device::run() {
	while (true) {

	

		for(auto& task : tasks)
		{
			task();
		}

		auto message = recieve();
		if (message != nullptr) {
			int c_id = message->c_id;
			auto channelIt = channels.find(c_id);

			if (channelIt != channels.end()) {
				auto channel = channelIt->second;
				channel->put(message);
			}
		}
	}
}



int Device::getDeviceFromName(std::string name) {
	//return broadcast(DEVICE_NAME_MATCH, name);
	for(auto& device : devices)
	{
		if (device->name == name) {
			return device->id;
		}
	}
	return 0;
}

int Device::getChannelFromName(int d_id, std::string name) {
	return 0;
}

std::shared_ptr<BaseChannel> Device::getRemoteChannel(int d_id, int c_id)
{
	return std::shared_ptr<BaseChannel>();
}

std::string Device::getDeviceAddress(int id) {
	return "";
}
