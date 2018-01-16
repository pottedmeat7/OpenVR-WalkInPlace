#include "IVRServerDriverHost005Hooks.h"

#include "../driver/ServerDriver.h"


namespace vrwalkinplace {
namespace driver {


HookData<IVRServerDriverHost005Hooks::trackedDeviceAdded_t> IVRServerDriverHost005Hooks::trackedDeviceAddedHook;
HookData<IVRServerDriverHost005Hooks::pollNextEvent_t> IVRServerDriverHost005Hooks::pollNextEventHook;


IVRServerDriverHost005Hooks::IVRServerDriverHost005Hooks(void* iptr) {
	if (!_isHooked) {
		CREATE_MH_HOOK(trackedDeviceAddedHook, _trackedDeviceAdded, "IVRServerDriverHost005::TrackedDeviceAdded", iptr, 0);
		CREATE_MH_HOOK(pollNextEventHook, _pollNextEvent, "IVRServerDriverHost005::PollNextEvent", iptr, 5);
		_isHooked = true;
	}
}


IVRServerDriverHost005Hooks::~IVRServerDriverHost005Hooks() {
	if (_isHooked) {
		REMOVE_MH_HOOK(trackedDeviceAddedHook);
		REMOVE_MH_HOOK(pollNextEventHook);
		_isHooked = false;
	}
}


std::shared_ptr<InterfaceHooks> IVRServerDriverHost005Hooks::createHooks(void * iptr) {
	std::shared_ptr<InterfaceHooks> retval = std::shared_ptr<InterfaceHooks>(new IVRServerDriverHost005Hooks(iptr));
	return retval;
}

bool IVRServerDriverHost005Hooks::_trackedDeviceAdded(void* _this, const char *pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, void *pDriver) {
	LOG(TRACE) << "IVRServerDriverHost005Hooks::_trackedDeviceAdded(" << _this << ", " << pchDeviceSerialNumber << ", " << eDeviceClass << ", " << pDriver << ")";
	serverDriver->hooksTrackedDeviceAdded(_this, 5, pchDeviceSerialNumber, eDeviceClass, pDriver);
	auto retval = trackedDeviceAddedHook.origFunc(_this, pchDeviceSerialNumber, eDeviceClass, pDriver);
	return retval;
}

bool IVRServerDriverHost005Hooks::_pollNextEvent(void* _this, void* pEvent, uint32_t uncbVREvent) {
	auto injectedEvent = serverDriver->getDriverEventForInjection(_this);
	if (injectedEvent.first) {
		if (injectedEvent.second == uncbVREvent) {
			memcpy(pEvent, injectedEvent.first.get(), uncbVREvent);
			auto event = (vr::VREvent_t*)pEvent;
			LOG(DEBUG) << "IVRServerDriverHost005Hooks::_pollNextEvent: Injecting event: " << event->eventType << ", " << event->trackedDeviceIndex;
			return true;
		} else {
			auto event = (vr::VREvent_t*)injectedEvent.first.get();
			LOG(ERROR) << "IVRServerDriverHost005Hooks::_pollNextEvent: Could not inject event (" << event->eventType << ", " << event->trackedDeviceIndex 
				<< ") because size does not match, expected " << uncbVREvent << " but got " << injectedEvent.second;
		}
	}
	bool retval, hretval;
	do {
		retval = pollNextEventHook.origFunc(_this, pEvent, uncbVREvent);
		if (retval) {
			hretval = serverDriver->hooksPollNextEvent(_this, 5, pEvent, uncbVREvent);
		}
	} while (retval && !hretval);
	return retval;
}


}
}
