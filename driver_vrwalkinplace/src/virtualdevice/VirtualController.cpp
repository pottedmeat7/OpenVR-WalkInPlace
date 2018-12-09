#include "VirtualController.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "../driver/ServerDriver.h"

namespace vrwalkinplace {
	namespace driver {



		VirtualController::VirtualController() {

		}

		VirtualController::VirtualController(const VirtualController &handle) {

		}

		VirtualController::VirtualController(std::string serial, bool side, vr::DriverPose_t initial_pose, vr::VRControllerState_t initial_state) :
			m_serialNumber(serial),
			leftSide(side),
			devicePose(initial_pose),
			currentState(initial_state)
		{
		}

		VirtualController::~VirtualController() {
		}

		vr::EVRInitError VirtualController::Activate(uint32_t unObjectId)
		{
			m_openvrId = unObjectId;
			m_propertyContainerHandle = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_openvrId);

			vr::VRProperties()->SetBoolProperty(m_propertyContainerHandle, vr::Prop_WillDriftInYaw_Bool, false);
			vr::VRProperties()->SetBoolProperty(m_propertyContainerHandle, vr::Prop_DeviceIsWireless_Bool, true);
			vr::VRProperties()->SetBoolProperty(m_propertyContainerHandle, vr::Prop_HasControllerComponent_Bool, true);

			vr::VRProperties()->SetInt32Property(m_propertyContainerHandle, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_Treadmill);
			
			vr::VRProperties()->SetInt32Property(m_propertyContainerHandle, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);
			vr::VRProperties()->SetInt32Property(m_propertyContainerHandle, vr::Prop_Axis1Type_Int32, vr::k_eControllerAxis_Trigger);
			vr::VRProperties()->SetInt32Property(m_propertyContainerHandle, vr::Prop_Axis2Type_Int32, vr::k_eControllerAxis_Joystick);

			vr::VRProperties()->SetStringProperty(m_propertyContainerHandle, vr::Prop_SerialNumber_String, m_serialNumber.c_str());
			//vr::VRProperties()->SetStringProperty(m_propertyContainerHandle, vr::Prop_ModelNumber_String, "Vive Controller MV");
			vr::VRProperties()->SetStringProperty(m_propertyContainerHandle, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5"); 
			//vr::VRProperties()->SetStringProperty(m_propertyContainerHandle, vr::Prop_ManufacturerName_String, "HTC");

			uint64_t available_buttons = vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) |
				vr::ButtonMaskFromId(vr::k_EButton_Knuckles_JoyStick) |
				vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger) |
				vr::ButtonMaskFromId(vr::k_EButton_Grip);

			vr::VRProperties()->SetUint64Property(m_propertyContainerHandle, vr::Prop_SupportedButtons_Uint64, available_buttons);
			
			vr::VRProperties()->SetStringProperty(m_propertyContainerHandle, vr::Prop_LegacyInputProfile_String, "legacy_bindings_ovrwip_controller");

			// Configure JSON controller configuration input profile
			vr::VRProperties()->SetStringProperty(m_propertyContainerHandle, vr::Prop_InputProfilePath_String, "{00vrwalkinplace}/input/ovrwip_controller_profile.json");
			
			// A button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad].insert(std::make_pair(ButtonEventType::ButtonPressed, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/trackpad/click", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::ButtonPressed]);

			// A button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad].insert(std::make_pair(ButtonEventType::ButtonTouched, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/trackpad/touch", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::ButtonTouched]);

			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad].insert(std::make_pair(ButtonEventType::ButtonUnpressed, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/trackpad/click", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::ButtonUnpressed]);

			// A button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad].insert(std::make_pair(ButtonEventType::ButtonUntouched, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/trackpad/touch", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::ButtonUntouched]);

			// A axis component
			m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad].insert(std::make_pair(ButtonEventType::TrackpadX, 0));
			CreateScalarComponent(m_propertyContainerHandle, "/input/trackpad/x", &m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::TrackpadX], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);

			// A axis component
			m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad].insert(std::make_pair(ButtonEventType::TrackpadY, 0));
			CreateScalarComponent(m_propertyContainerHandle, "/input/trackpad/y", &m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::TrackpadY], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);

			// Joystick button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick].insert(std::make_pair(ButtonEventType::ButtonPressed, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/joystick/click", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_A][ButtonEventType::ButtonPressed]);

			// Joystick button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick].insert(std::make_pair(ButtonEventType::ButtonTouched, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/joystick/touch", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_A][ButtonEventType::ButtonTouched]);

			// Joystick button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick].insert(std::make_pair(ButtonEventType::ButtonUnpressed, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/joystick/click", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_A][ButtonEventType::ButtonUnpressed]);

			// Joystick button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick].insert(std::make_pair(ButtonEventType::ButtonUntouched, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/joystick/touch", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_A][ButtonEventType::ButtonUntouched]);

			// joystick axis component
			m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick].insert(std::make_pair(ButtonEventType::JoystickX, 0));
			CreateScalarComponent(m_propertyContainerHandle, "/input/joystick/x", &m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick][ButtonEventType::JoystickX], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);

			m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick].insert(std::make_pair(ButtonEventType::JoystickY, 0));
			CreateScalarComponent(m_propertyContainerHandle, "/input/joystick/y", &m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick][ButtonEventType::JoystickY], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);

			// Trigger button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Trigger].insert(std::make_pair(ButtonEventType::ButtonPressed, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/trigger/click", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Trigger][ButtonEventType::ButtonPressed]);

			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Trigger].insert(std::make_pair(ButtonEventType::ButtonTouched, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/trigger/touch", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Trigger][ButtonEventType::ButtonTouched]);

			// trigger axis component
			m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Trigger].insert(std::make_pair(ButtonEventType::TriggerX, 0));
			CreateScalarComponent(m_propertyContainerHandle, "/input/trigger/x", &m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Trigger][ButtonEventType::TriggerX], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);

			// Grip button component
			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Grip].insert(std::make_pair(ButtonEventType::ButtonPressed, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/grip/click", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Grip][ButtonEventType::ButtonPressed]);

			m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Grip].insert(std::make_pair(ButtonEventType::ButtonTouched, 0));
			CreateBooleanComponent(m_propertyContainerHandle, "/input/grip/touch", &m_ulBoolComponentsMap[vr::EVRButtonId::k_EButton_Grip][ButtonEventType::ButtonTouched]);

			// app button component
			return vr::EVRInitError::VRInitError_None;
		}

		void VirtualController::Deactivate()
		{
		}

		void VirtualController::EnterStandby()
		{
		}

		void * VirtualController::GetComponent(const char * pchComponentNameAndVersion)
		{
			if (0 == strcmp(vr::IVRDriverInput_Version, pchComponentNameAndVersion))
			{
				return (vr::IVRDriverInput*)this;
			}

			return NULL;

		}

		void VirtualController::DebugRequest(const char * pchRequest, char * pchResponseBuffer, uint32_t unResponseBufferSize)
		{
			if (unResponseBufferSize >= 1)
				pchResponseBuffer[0] = 0;
		}


		void VirtualController::RunFrame() {
		}

		vr::DriverPose_t VirtualController::GetPose()
		{
			return devicePose;
		}
		
		void VirtualController::mapInputDevice(uint32_t unObjectId, bool leftRole) {
			//m_openvrId = unObjectId;
			//m_propertyContainerHandle = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_openvrId);
		}

		void VirtualController::updatePose(vr::DriverPose_t new_pose)
		{
			//LOG(INFO) << "new pose values - pos(x,y,z): (" << new_pose.vecPosition[0] << "," << new_pose.vecPosition[1] << "," << new_pose.vecPosition[2] << ") - vel(x,y,x): (" << new_pose.vecVelocity[0] << "," << new_pose.vecVelocity[1] << "," << new_pose.vecVelocity[2] << ")";
			devicePose.vecWorldFromDriverTranslation[0] = new_pose.vecWorldFromDriverTranslation[0];
			devicePose.vecWorldFromDriverTranslation[1] = new_pose.vecWorldFromDriverTranslation[1];
			devicePose.vecWorldFromDriverTranslation[2] = new_pose.vecWorldFromDriverTranslation[2];
			devicePose.vecVelocity[0] = new_pose.vecVelocity[0];
			devicePose.vecVelocity[1] = new_pose.vecVelocity[1];
			devicePose.vecVelocity[2] = new_pose.vecVelocity[2];
			devicePose.qRotation = new_pose.qRotation;
			//devicePose = new_pose;
		}

		void VirtualController::updateState(vr::VRControllerState_t new_state) {
			uint64_t ulChangedTouched = new_state.ulButtonTouched ^ currentState.ulButtonTouched;
			uint64_t ulChangedPressed = new_state.ulButtonPressed ^ currentState.ulButtonPressed;
			buttonUpdates(ButtonEventType::ButtonUnpressed, ulChangedTouched & ~new_state.ulButtonTouched);
			buttonUpdates(ButtonEventType::ButtonUntouched, ulChangedPressed & ~new_state.ulButtonPressed);
			buttonUpdates(ButtonEventType::ButtonTouched, ulChangedTouched & new_state.ulButtonTouched);
			buttonUpdates(ButtonEventType::ButtonPressed, ulChangedPressed & new_state.ulButtonPressed);

			new_state.unPacketNum = currentState.unPacketNum + 1;
			currentState = new_state;
		}

		void VirtualController::buttonUpdates(ButtonEventType eventType, uint64_t ulMask) {
			if (!ulMask)
				return;

			for (int i = 0; i < vr::k_EButton_Max; i++)
			{
				vr::EVRButtonId button = (vr::EVRButtonId)i;

				uint64_t bit = ButtonMaskFromId(button);

				if (bit & ulMask)
				{
					sendButtonEvent(eventType, button, 0);
				}
			}
		}

		vr::EVRInputError VirtualController::CreateBooleanComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, vr::VRInputComponentHandle_t *pHandle) {
			return vr::VRDriverInput()->CreateBooleanComponent(ulContainer, pchName, pHandle);
		}

		vr::EVRInputError VirtualController::UpdateBooleanComponent(vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) {
			return vr::VRDriverInput()->UpdateBooleanComponent(ulComponent, bNewValue, fTimeOffset);
		}

		vr::EVRInputError VirtualController::CreateScalarComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, vr::VRInputComponentHandle_t *pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) {
			return vr::VRDriverInput()->CreateScalarComponent(ulContainer, pchName, pHandle, eType, eUnits);
		}

		vr::EVRInputError VirtualController::UpdateScalarComponent(vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) {
			return vr::VRDriverInput()->UpdateScalarComponent(ulComponent, fNewValue, fTimeOffset);
		}

		vr::EVRInputError VirtualController::CreateHapticComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, vr::VRInputComponentHandle_t *pHandle) {
			return vr::EVRInputError::VRInputError_None;
		}

		vr::EVRInputError VirtualController::CreateSkeletonComponent(vr::PropertyContainerHandle_t ulContainer, const char *pchName, const char *pchSkeletonPath, const char *pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t *pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t *pHandle) {
			return vr::EVRInputError::VRInputError_None;
		}

		vr::EVRInputError VirtualController::UpdateSkeletonComponent(vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t *pTransforms, uint32_t unTransformCount) {
			return vr::EVRInputError::VRInputError_None;
		}

		void VirtualController::sendButtonEvent(ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			auto it = m_ulBoolComponentsMap.find(eButtonId);
			if (it != m_ulBoolComponentsMap.end()) {
				auto findType = (ButtonEventType::ButtonUnpressed == eventType) ? ButtonEventType::ButtonPressed : (eventType == ButtonEventType::ButtonUntouched) ? ButtonEventType::ButtonTouched : eventType;
				auto eIt = m_ulBoolComponentsMap[eButtonId].find(findType);
				if (eIt != m_ulBoolComponentsMap[eButtonId].end()) {
					uint64_t componentHandle = m_ulBoolComponentsMap[eButtonId][findType];
					bool boolVal = (eventType == ButtonEventType::ButtonPressed || eventType == ButtonEventType::ButtonTouched);
					vr::EVRInputError eVRIError = UpdateBooleanComponent(componentHandle, boolVal, eventTimeOffset);
					//LOG(INFO) << "apply boolean event " << (uint32_t)eventType << " on button " << eButtonId << " on device " << m_openvrId;
					if (eVRIError != vr::EVRInputError::VRInputError_None) {
						LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
					}
				}
				else {
					LOG(WARNING) << "Device " << m_openvrId << ": No mapping with button id  " << eButtonId << " with event type " << (int)eventType << " to input component.";
				}
			}
			else {
				LOG(WARNING) << "Device " << m_openvrId << ": No mapping from button id " << eButtonId << " to input component.";
			}
		}

		void VirtualController::sendAxisEvent(uint32_t eButtonId, const vr::VRControllerAxis_t& axisState) {
			auto it = m_ulScalarComponentsMap.find(eButtonId);
			if (it != m_ulScalarComponentsMap.end()) {
				if (eButtonId == vr::EVRButtonId::k_EButton_SteamVR_Touchpad) {
					try {
						vr::EVRInputError eVRIError = UpdateScalarComponent(m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::TrackpadX], axisState.x, 0);
						if (eVRIError != vr::EVRInputError::VRInputError_None) {
							LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
						}
						eVRIError = UpdateScalarComponent(m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Touchpad][ButtonEventType::TrackpadY], axisState.y, 0);
						//LOG(INFO) << "apply axis event " << eButtonId << " X dimension on device " << m_openvrId;
						if (eVRIError != vr::EVRInputError::VRInputError_None) {
							LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
						}
					}
					catch (std::exception& e) {
						LOG(INFO) << "AXIS EVENT ERROR CAUGHT " << e.what();
					}
				}
				else if (eButtonId == vr::EVRButtonId::k_EButton_Knuckles_JoyStick) {
					vr::EVRInputError eVRIError = UpdateScalarComponent(m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick][ButtonEventType::JoystickX], axisState.x, 0);
					if (eVRIError != vr::EVRInputError::VRInputError_None) {
						LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
					}
					eVRIError = UpdateScalarComponent(m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_Knuckles_JoyStick][ButtonEventType::JoystickY], axisState.y, 0);
					//LOG(INFO) << "apply axis event " << unWhichAxis << " Y dimension on device " << m_openvrId;
					if (eVRIError != vr::EVRInputError::VRInputError_None) {
						LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
					}
				}
				else if (eButtonId == vr::EVRButtonId::k_EButton_SteamVR_Trigger) {
					vr::EVRInputError eVRIError = UpdateScalarComponent(m_ulScalarComponentsMap[vr::EVRButtonId::k_EButton_SteamVR_Trigger][ButtonEventType::TriggerX], axisState.x, 0);
					if (eVRIError != vr::EVRInputError::VRInputError_None) {
						LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
					}
				}
			}
			else {
				LOG(WARNING) << "Device " << m_openvrId << ": No mapping from axis id " << eButtonId << " to input component.";
			}
		}
		bool _matchInputComponentName(const char* name, std::string& segment0, std::string& segment1, std::string& segment2, std::string& segment3) {
			boost::regex rgx("^/([^/]*)(/([^/]*))?(/([^/]*))?(/([^/]*))?");
			boost::smatch match;
			std::string text(name);
			if (boost::regex_search(text, match, rgx)) {
				segment0 = match[1];
				segment1 = match[3];
				segment2 = match[5];
				segment3 = match[7];
				return true;
			}
			else {
				return false;
			}
		}

	} // end namespace driver
} // end namespace vrwalkinplace
