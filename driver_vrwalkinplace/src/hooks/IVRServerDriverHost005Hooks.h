#pragma once

#include "common.h"
#include <memory>
#include <openvr_driver.h>


namespace vrwalkinplace {
namespace driver {


class IVRServerDriverHost005Hooks : public InterfaceHooks {
public:
	typedef bool(*trackedDeviceAdded_t)(void*, const char*, vr::ETrackedDeviceClass, void*);
	typedef bool(*pollNextEvent_t)(void*, void*, uint32_t);

	static std::shared_ptr<InterfaceHooks> createHooks(void* iptr);
	virtual ~IVRServerDriverHost005Hooks();

private:
	bool _isHooked = false;

	IVRServerDriverHost005Hooks(void* iptr);

	static HookData<trackedDeviceAdded_t> trackedDeviceAddedHook;
	static HookData<pollNextEvent_t> pollNextEventHook;

	static bool _trackedDeviceAdded(void* _this, const char *pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, void *pDriver);
	static bool _pollNextEvent(void* _this, void* pEvent, uint32_t uncbVREvent);

};

}
}
