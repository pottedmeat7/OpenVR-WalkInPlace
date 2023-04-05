#include "driver/ServerDriver.h"
#include "driver/WatchdogProvider.h"
#include "logging.h"

const char* logConfigFileName = "logging.conf";

const char* logConfigDefault =
"* GLOBAL:\n"
"	FORMAT = \"[%level] %datetime{%Y-%M-%d %H:%m:%s}: %msg\"\n"
"	FILENAME = \"driver_vrwalkinplace.log\"\n"
"	ENABLED = true\n"
"	TO_FILE = true\n"
"	TO_STANDARD_OUTPUT = true\n"
"	MAX_LOG_FILE_SIZE = 2097152 ## 2MB\n"
"* TRACE:\n"
"	ENABLED = false\n"
"* DEBUG:\n"
"	ENABLED = false\n";

INITIALIZE_EASYLOGGINGPP

void init_logging() {
	el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
	el::Configurations conf(logConfigFileName);
	conf.parseFromText(logConfigDefault);
	conf.parseFromFile(logConfigFileName);
	conf.setRemainingToDefault();
	el::Loggers::reconfigureAllLoggers(conf);
}

static vrwalkinplace::driver::ServerDriver serverDriver;
static vrwalkinplace::driver::WatchdogProvider watchdogProvider;

#if defined(_WIN32)
#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#elif defined(__GNUC__) || defined(COMPILER_GCC) || defined(__APPLE__)
#define HMD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
#define HMD_DLL_IMPORT extern "C" 
#else
#error "Unsupported Platform."
#endif

HMD_DLL_EXPORT void *HmdDriverFactory(const char *pInterfaceName, int *pReturnCode) {
	init_logging();
	LOG(TRACE) << "HmdDriverFactory( " << pInterfaceName << " )";
	if (std::strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName) == 0){
		return &serverDriver;
	} else if (std::strcmp(vr::ITrackedDeviceServerDriver_Version, pInterfaceName) == 0) {
		return &serverDriver;
	} else if (std::strcmp(vr::IVRWatchdogProvider_Version, pInterfaceName) == 0) {
		return &watchdogProvider;
	}
	if (pReturnCode) {
		*pReturnCode = vr::VRInitError_Init_InterfaceNotFound;
	}
	return nullptr;
}
