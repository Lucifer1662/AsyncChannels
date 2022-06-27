#include "LightSwitchEntity.h"
#include "Channel.h"

void LightSwitchEntity::run() {
	turnOnOrOff->send(true);
}
