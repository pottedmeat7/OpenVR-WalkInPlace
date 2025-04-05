#include "driver/ServerDriver.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "driver/WatchdogProvider.h"
static vrwalkinplace::driver::WatchdogProvider watchdogProvider;
#endif

static vrwalkinplace::driver::ServerDriver serverDriver;

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif

#include <cstdlib>
#include <iostream>

#define LOG(text) std::cerr << text << std::endl;

HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode) {
	if(isWindows) {
		LOG("HmdDriverFactory( " << pInterfaceName << " )");
	}
	if (std::strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName) == 0){
		return &serverDriver;
	} else if (isWindows) {
		if (std::strcmp(vr::ITrackedDeviceServerDriver_Version, pInterfaceName) == 0) {
			return &serverDriver;
		} else if (std::strcmp(vr::IVRWatchdogProvider_Version, pInterfaceName) == 0) {
			return &watchdogProvider;
		}
	}
	if (pReturnCode) {
		*pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
	}
	return nullptr;
}
