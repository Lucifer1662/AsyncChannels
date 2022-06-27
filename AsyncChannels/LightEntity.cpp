#include "LightEntity.h"
#include "Channel.h"

void LightEntity::run() {
	while (true) {
		auto state = turnOnOrOff->recieve();
		isOn = state;
	}

}
