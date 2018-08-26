#include "DeviceManipulationHandle.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "../driver/ServerDriver.h"
#include "../hooks/IVRServerDriverHost004Hooks.h"
#include "../hooks/IVRServerDriverHost005Hooks.h"
#include "../hooks/IVRDriverInput001Hooks.h"

#undef WIN32_LEAN_AND_MEAN
#undef NOSOUND
#include <Windows.h>
// According to windows documentation mmsystem.h should be automatically included with Windows.h when WIN32_LEAN_AND_MEAN and NOSOUND are not defined
// But it doesn't work so I have to include it manually
#include <mmsystem.h>


namespace vrwalkinplace {
	namespace driver {


#define VECTOR_ADD(lhs, rhs) \
		lhs[0] += rhs.v[0]; \
		lhs[1] += rhs.v[1]; \
		lhs[2] += rhs.v[2];


		DeviceManipulationHandle::DeviceManipulationHandle(const char* serial, vr::ETrackedDeviceClass eDeviceClass, void* driverPtr, void* driverHostPtr, int driverInterfaceVersion)
			: m_isValid(true), m_parent(ServerDriver::getInstance()), m_deviceDriverPtr(driverPtr), m_deviceDriverHostPtr(driverHostPtr),
			m_deviceDriverInterfaceVersion(driverInterfaceVersion), m_eDeviceClass(eDeviceClass), m_serialNumber(serial) {
			memset(_AxisIdToComponentHandleMap, 0, sizeof(_AxisIdToComponentHandleMap));
		}

		void DeviceManipulationHandle::ll_sendButtonEvent(ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			auto it = _ButtonIdToComponentHandleMap.find(eButtonId);
			if (it != _ButtonIdToComponentHandleMap.end()) {
				uint64_t componentHandle = 0;
				bool newValue = false;
				switch (eventType) {
				case ButtonEventType::ButtonTouched:
					componentHandle = it->second.first;
					newValue = true;
					break;
				case ButtonEventType::ButtonUntouched:
					componentHandle = it->second.first;
					newValue = false;
					break;
				case ButtonEventType::ButtonPressed:
					componentHandle = it->second.second;
					newValue = true;
					break;
				case ButtonEventType::ButtonUnpressed:
					componentHandle = it->second.second;
					newValue = false;
					break;
				default:
					break;
				}
				if (componentHandle != 0) {
					vr::EVRInputError eVRIError = vr::VRDriverInput()->UpdateBooleanComponent(componentHandle, newValue, eventTimeOffset);
					LOG(INFO) << "apply boolean event " << eButtonId << " on device " << m_openvrId;
					if (eVRIError != vr::EVRInputError::VRInputError_None) {
						LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
					}
					//IVRDriverInput001Hooks::updateBooleanComponentOrig(m_driverInputPtr, componentHandle, newValue, eventTimeOffset);
				}
			}
			else {
				LOG(WARNING) << "Device " << m_openvrId << ": No mapping from button id " << eButtonId << " to input component.";
			}
		}

		void DeviceManipulationHandle::ll_sendAxisEvent(uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState) {
			if (unWhichAxis < 5) {
				if (_AxisIdToComponentHandleMap[unWhichAxis].first == 0 && _AxisIdToComponentHandleMap[unWhichAxis].second == 0) {
					LOG(WARNING) << "Device " << m_openvrId << ": No mapping from axis id " << unWhichAxis << " to input component.";
				}
				else {
					if (_AxisIdToComponentHandleMap[unWhichAxis].first != 0) {
						//sendScalarComponentUpdate(m_openvrId, unWhichAxis, 0, axisState.x, 0.0);
						vr::EVRInputError eVRIError = vr::VRDriverInput()->UpdateScalarComponent(_AxisIdToComponentHandleMap[unWhichAxis].first, axisState.x, 0);
						LOG(INFO) << "apply axis event " << unWhichAxis << " X dimension on device " << m_openvrId;
						if (eVRIError != vr::EVRInputError::VRInputError_None) {
							LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
						}
					}
					if (_AxisIdToComponentHandleMap[unWhichAxis].second != 0) {
						//sendScalarComponentUpdate(m_openvrId, unWhichAxis, 1, axisState.y, 0.0);
						vr::EVRInputError eVRIError = vr::VRDriverInput()->UpdateScalarComponent(_AxisIdToComponentHandleMap[unWhichAxis].second, axisState.y, 0);
						LOG(INFO) << "apply axis event " << unWhichAxis << " Y dimension on device " << m_openvrId;
						if (eVRIError != vr::EVRInputError::VRInputError_None) {
							LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
						}
					}
				}
			}
		}

		void DeviceManipulationHandle::RunFrame() {
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

		std::map<std::string, vr::EVRButtonId> _inputComponentNameToButtonId = {
			{ "system", vr::k_EButton_System },
			{ "application_menu", vr::k_EButton_ApplicationMenu },
			{ "grip", vr::k_EButton_Grip },
			{ "dpad_left/", vr::k_EButton_DPad_Left },
			{ "dpad_up", vr::k_EButton_DPad_Up },
			{ "dpad_right", vr::k_EButton_DPad_Right },
			{ "dpad_down", vr::k_EButton_DPad_Down },
			{ "a", vr::k_EButton_A },
			{ "x", vr::k_EButton_A },
			{ "b", vr::k_EButton_ApplicationMenu },
			{ "y", vr::k_EButton_ApplicationMenu },
			{ "trackpad", vr::k_EButton_SteamVR_Touchpad },
			{ "joystick", vr::k_EButton_Axis2 },
			{ "trigger", vr::k_EButton_SteamVR_Trigger },
		};

		std::map<std::string, uint32_t> _inputComponentNameToAxisId = {
			{ "trackpad", 0 },
			{ "trigger", 1 },
			{ "joystick", 2 },
		};

		void DeviceManipulationHandle::inputAddBooleanComponent(const char *pchName, uint64_t pHandle) {
			std::string sg0, sg1, sg2, sg3;
			if (_matchInputComponentName(pchName, sg0, sg1, sg2, sg3)) {
				LOG(DEBUG) << "Device Component Name Segments: \"" << sg0 << "\", \"" << sg1 << "\", \"" << sg2 << "\", \"" << sg3 << "\"";
				vr::EVRButtonId buttonId;
				int buttonType = 1; // 0 .. touch, 1 ..click
				bool errorFlag = false;
				if (!sg3.empty()) {
					LOG(ERROR) << "Device input component name \"" << pchName << "\" has too many segments.";
				}
				else {
					if (boost::iequals(sg0, "proximity")) { // proximity sensor
						buttonId = vr::k_EButton_ProximitySensor;
					}
					else if (boost::iequals(sg0, "input")) { // digital button
						boost::algorithm::to_lower(sg1);
						auto it = _inputComponentNameToButtonId.find(sg1);
						if (it != _inputComponentNameToButtonId.end()) {
							buttonId = it->second;
							if (boost::iequals(sg2, "touch")) {
								buttonType = 0;
							}
						}
						else {
							LOG(ERROR) << "Could not map input component name \"" << pchName << "\" to a button id.";
							errorFlag = true;
						}
					}
					else {
						LOG(ERROR) << "Unknown first component name segment \"" << sg0 << "\".";
						errorFlag = true;
					}
				}
				if (!errorFlag) {
					_componentHandleToButtonIdMap.emplace(pHandle, std::make_pair(buttonId, buttonType));
					if (buttonType == 0) {
						_ButtonIdToComponentHandleMap[buttonId].first = pHandle;
					}
					else {
						_ButtonIdToComponentHandleMap[buttonId].second = pHandle;
					}
					LOG(INFO) << "Mapped input component \"" << pchName << "\" on device " << m_openvrId << " to button id (" << (int)buttonId << ", " << buttonType << ")";
				}
			}
			else {
				LOG(ERROR) << "Could not parse input component name \"" << pchName << "\".";
			}
		}

		void DeviceManipulationHandle::inputAddScalarComponent(const char *pchName, uint64_t pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) {
			std::string sg0, sg1, sg2, sg3;
			if (_matchInputComponentName(pchName, sg0, sg1, sg2, sg3)) {
				LOG(DEBUG) << "Device Component Name Segments: \"" << sg0 << "\", \"" << sg1 << "\", \"" << sg2 << "\", \"" << sg3 << "\"";
				uint32_t axisId;
				uint32_t axisDim = 0;
				bool errorFlag = false;
				if (!sg3.empty()) {
					LOG(ERROR) << "Device input component name \"" << pchName << "\" has too many segments.";
				}
				else {
					if (boost::iequals(sg0, "input")) { // analog input
						boost::algorithm::to_lower(sg1);
						auto it = _inputComponentNameToAxisId.find(sg1);
						if (it != _inputComponentNameToAxisId.end()) {
							axisId = it->second;
							if (boost::iequals(sg2, "x")) {
								axisDim = 0;
							}
							else if (boost::iequals(sg2, "y")) {
								axisDim = 1;
							}
						}
						else {
							LOG(ERROR) << "Could not map input component name \"" << pchName << "\" to an axis id.";
							errorFlag = true;
						}
					}
					else {
						LOG(ERROR) << "Unknown first component name segment \"" << sg0 << "\".";
						errorFlag = true;
					}
				}
				if (!errorFlag) {
					_componentHandleToAxisIdMap.emplace(pHandle, std::make_pair(axisId, axisDim));
					if (axisDim == 0) {
						_AxisIdToComponentHandleMap[axisId].first = pHandle;
					}
					else {
						_AxisIdToComponentHandleMap[axisId].second = pHandle;
					}
					LOG(INFO) << "Mapped input component \"" << pchName << "\" on device " << m_openvrId << " to axis id (" << axisId << ", " << axisDim << ")" << " with handle " << pHandle;
				}
			}
			else {
				LOG(ERROR) << "Could not parse input component name \"" << pchName << "\".";
			}
		}


	} // end namespace driver
} // end namespace vrwalkinplace
