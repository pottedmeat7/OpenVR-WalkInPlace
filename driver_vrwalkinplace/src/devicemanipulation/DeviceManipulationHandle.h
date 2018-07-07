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
	bool flipYaw = false;
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

	std::map<uint64_t, std::pair<vr::EVRButtonId, int>> _componentHandleToButtonIdMap;
	std::map<vr::EVRButtonId, std::pair<uint64_t, uint64_t>> _ButtonIdToComponentHandleMap;
	std::map<uint64_t, std::pair<unsigned, unsigned>> _componentHandleToAxisIdMap;
	std::pair<uint64_t, uint64_t> _AxisIdToComponentHandleMap[5];


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

	void setFlipYaw(bool flipYaw);
	void ll_sendPoseUpdate(const vr::DriverPose_t& newPose);
	void ll_sendButtonEvent(ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset);
	void ll_sendAxisEvent(uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState);
	void ll_sendScalarComponentUpdate(vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset);

	bool handlePoseUpdate(uint32_t& unWhichDevice, vr::DriverPose_t& newPose, uint32_t unPoseStructSize);
	bool handleButtonEvent(uint32_t& unWhichDevice, ButtonEventType eventType, vr::EVRButtonId& eButtonId, double& eventTimeOffset);
	bool handleAxisUpdate(uint32_t& unWhichDevice, uint32_t& unWhichAxis, vr::VRControllerAxis_t& axisState);
	bool handleBooleanComponentUpdate(vr::VRInputComponentHandle_t& ulComponent, bool& bNewValue, double& fTimeOffset);
	bool handleScalarComponentUpdate(vr::VRInputComponentHandle_t& ulComponent, float& fNewValue, double& fTimeOffset);

	void sendButtonEvent(uint32_t unWhichDevice, ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset);
	void sendAxisEvent(uint32_t unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState);
	void sendScalarComponentUpdate(uint32_t unWhichDevice, uint32_t unWhichAxis, uint32_t unAxisDim, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset);
	void sendScalarComponentUpdate(uint32_t unWhichDevice, uint32_t unWhichAxis, uint32_t unAxisDim, float, double fTimeOffset);
	
	void inputAddBooleanComponent(const char *pchName, uint64_t pHandle);
	void inputAddScalarComponent(const char *pchName, uint64_t pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits);

	void setPropertyContainer(vr::PropertyContainerHandle_t container) { m_propertyContainerHandle = container; }
	vr::PropertyContainerHandle_t propertyContainer() { return m_propertyContainerHandle; }

	void RunFrame();


};


} // end namespace driver
} // end namespace vrwalkinplace
