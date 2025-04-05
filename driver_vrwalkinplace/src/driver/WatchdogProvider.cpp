#include "WatchdogProvider.h"

#include <cstdlib>
#include <iostream>

#define LOG(text) std::cerr << text << std::endl;

// driver namespace
namespace vrwalkinplace {
namespace driver {


vr::EVRInitError WatchdogProvider::Init(vr::IVRDriverContext * pDriverContext) {
	LOG("WatchdogProvider::Init()");
	return vr::EVRInitError::VRInitError_None;
}


void WatchdogProvider::Cleanup() {
	LOG("WatchdogProvider::Cleanup()");
}


}
}
