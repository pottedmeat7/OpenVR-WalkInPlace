#pragma once
#pragma once

#include <openvr_driver.h>
#include <vrwalkinplace_types.h>
#include <openvr_math.h>
#include "../logging.h"
#include "../hooks/common.h"



// driver namespace
namespace vrwalkinplace {
namespace driver {


// forward declarations
class ServerDriver;
class InterfaceHooks;
class MotionCompensationManager;


// Stores manipulation information about an openvr device
class DeviceManipulationHandle {
private:
	bool m_isValid = false;
	ServerDriver* m_parent;
	std::recursive_mutex _mutex;
	vr::ETrackedDeviceClass m_eDeviceClass = vr::TrackedDeviceClass_Invalid;
	uint32_t m_openvrId = vr::k_unTrackedDeviceIndexInvalid;
	std::string m_serialNumber;


	int m_deviceDriverInterfaceVersion = 0;
	void* m_deviceDriverPtr;
	void* m_deviceDriverHostPtr;
	void* m_driverInputPtr = nullptr;
	std::shared_ptr<InterfaceHooks> m_serverDriverHooks;
	std::shared_ptr<InterfaceHooks> m_controllerComponentHooks;

	int m_deviceMode = 0; 
	bool _disconnectedMsgSend = false;

	vr::HmdQuaternion_t yawQRotation;

	vr::PropertyContainerHandle_t m_propertyContainerHandle = vr::k_ulInvalidPropertyContainer;

	std::map<vr::VRInputComponentHandle_t, std::pair<vr::EVRButtonId, int>> _componentHandleToButtonIdMap;
	std::map<vr::EVRButtonId, std::pair<vr::VRInputComponentHandle_t, vr::VRInputComponentHandle_t>> _ButtonIdToComponentHandleMap;
	std::map<vr::VRInputComponentHandle_t, std::pair<unsigned, unsigned>> _componentHandleToAxisIdMap;
	std::pair<vr::VRInputComponentHandle_t, vr::VRInputComponentHandle_t> _AxisIdToComponentHandleMap[5];


public:
	DeviceManipulationHandle(const char* serial, vr::ETrackedDeviceClass eDeviceClass, void* driverPtr, void* driverHostPtr, int driverInterfaceVersion);

	bool isValid() const { return m_isValid; }
	vr::ETrackedDeviceClass deviceClass() const { return m_eDeviceClass; }
	uint32_t openvrId() const { return m_openvrId; }
	void setOpenvrId(uint32_t id) { m_openvrId = id; }
	const std::string& serialNumber() { return m_serialNumber; }

	void* driverPtr() const { return m_deviceDriverPtr; }
	void* driverHostPtr() const { return m_deviceDriverHostPtr; }
	void* driverInputPtr() const { return m_driverInputPtr; }
	void setDriverInputPtr(void* ptr) { m_driverInputPtr = ptr; }
	void setServerDriverHooks(std::shared_ptr<InterfaceHooks> hooks) { m_serverDriverHooks = hooks; }
	void setControllerComponentHooks(std::shared_ptr<InterfaceHooks> hooks) { m_controllerComponentHooks = hooks; }

	int deviceMode() const { return m_deviceMode; }

	void ll_sendButtonEvent(ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset);
	void ll_sendAxisEvent(uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState);
	
	void inputAddBooleanComponent(const char *pchName, uint64_t * pHandle);
	void inputAddScalarComponent(const char *pchName, uint64_t * pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits);

	void setPropertyContainer(vr::PropertyContainerHandle_t container) { m_propertyContainerHandle = container; }
	vr::PropertyContainerHandle_t propertyContainer() { return m_propertyContainerHandle; }

	void RunFrame();


};


} // end namespace driver
} // end namespace vrwalkinplace
