#pragma once
#pragma once

#include <openvr_driver.h>
#include <vrwalkinplace_types.h>
#include <openvr_math.h>
#include "../logging.h"


// driver namespace
namespace vrwalkinplace {
namespace driver {


// Stores manipulation information about an openvr device
class VirtualController : public vr::ITrackedDeviceServerDriver, public vr::IVRDriverInput {
private:
	bool m_isValid = false;
	vr::DriverPose_t devicePose;
	bool leftSide = false;
	vr::VRControllerState_t currentState;
	vr::ETrackedDeviceClass m_eDeviceClass = vr::TrackedDeviceClass_Invalid;
	uint32_t m_openvrId = vr::k_unTrackedDeviceIndexInvalid;
	std::string m_serialNumber;
	
	vr::PropertyContainerHandle_t m_propertyContainerHandle = vr::k_ulInvalidPropertyContainer;

	std::map<vr::VRInputComponentHandle_t, std::pair<vr::EVRButtonId, int>> _componentHandleToButtonIdMap;
	std::map<vr::EVRButtonId, std::pair<vr::VRInputComponentHandle_t, vr::VRInputComponentHandle_t>> _ButtonIdToComponentHandleMap;
	std::map<vr::VRInputComponentHandle_t, std::pair<unsigned, unsigned>> _componentHandleToAxisIdMap;
	std::pair<vr::VRInputComponentHandle_t, vr::VRInputComponentHandle_t> _AxisIdToComponentHandleMap[5];


public:
	VirtualController();
	VirtualController(const VirtualController &handle);
	~VirtualController();
	VirtualController(std::string serial, bool side, vr::DriverPose_t initial_pose, vr::VRControllerState_t initial_state);

	const std::string& serialNumber() { return m_serialNumber; }
	uint32_t openvrId() { return m_openvrId; }

	void sendButtonEvent(ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset);
	void sendAxisEvent(uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState);
	void mapInputDevice(uint32_t unWhichDevice, bool leftMode);
	void updatePose(vr::DriverPose_t new_pose);
	void updateState(vr::VRControllerState_t new_state);

	std::map<uint64_t, std::map<ButtonEventType, vr::VRInputComponentHandle_t> > m_ulBoolComponentsMap;
	std::map<uint64_t, std::map<ButtonEventType, vr::VRInputComponentHandle_t> > m_ulScalarComponentsMap;

	typedef enum
	{
		k_eAxis_Trackpad_X,
		k_eAxis_Trackpad_Y,
		k_eAxis_Joystick_X,
		k_eAxis_Joystick_Y,
	} PossibleAxisComponents;


	void setPropertyContainer(vr::PropertyContainerHandle_t container) { m_propertyContainerHandle = container; }
	vr::PropertyContainerHandle_t propertyContainer() { return m_propertyContainerHandle; }

	void RunFrame();

	// Inherited via ITrackedDeviceServerDriver
	virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
	virtual void Deactivate() override;
	virtual void EnterStandby() override;
	virtual void * GetComponent(const char * pchComponentNameAndVersion) override;
	virtual void DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize) override;
	virtual vr::DriverPose_t GetPose() override;
	//Inherited via IVRDriverInputvirtual
	virtual vr::EVRInputError CreateBooleanComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, vr::VRInputComponentHandle_t *pHandle) override;
	virtual vr::EVRInputError UpdateBooleanComponent(vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) override;
	virtual vr::EVRInputError CreateScalarComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, vr::VRInputComponentHandle_t *pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) override;
	virtual vr::EVRInputError UpdateScalarComponent(vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) override;
	virtual vr::EVRInputError CreateHapticComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, vr::VRInputComponentHandle_t *pHandle) override;
	virtual vr::EVRInputError CreateSkeletonComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, const char *pchSkeletonPath, const char *pchBasePosePath, const vr::VRBoneTransform_t *pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t *pHandle) override;
	virtual vr::EVRInputError UpdateSkeletonComponent(vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t *pTransforms, uint32_t unTransformCount) override;


};


} // end namespace driver
} // end namespace vrwalkinplace
