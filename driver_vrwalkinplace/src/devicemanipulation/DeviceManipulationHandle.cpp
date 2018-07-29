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

bool DeviceManipulationHandle::handlePoseUpdate(uint32_t& unWhichDevice, vr::DriverPose_t& newPose, uint32_t unPoseStructSize) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	ll_sendPoseUpdate(newPose);
	return true;
}

void DeviceManipulationHandle::setFlipYaw(bool val) {
	flipYaw = val;
}

void DeviceManipulationHandle::ll_sendPoseUpdate(const vr::DriverPose_t& newPose) {
	if (m_deviceDriverInterfaceVersion == 4) {
		IVRServerDriverHost004Hooks::trackedDevicePoseUpdatedOrig(m_deviceDriverHostPtr, m_openvrId, newPose, sizeof(vr::DriverPose_t));
	}
	else if (m_deviceDriverInterfaceVersion == 5) {
		IVRServerDriverHost005Hooks::trackedDevicePoseUpdatedOrig(m_deviceDriverHostPtr, m_openvrId, newPose, sizeof(vr::DriverPose_t));
	}
}

void DeviceManipulationHandle::ll_sendButtonEvent(ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {
	if (m_deviceDriverInterfaceVersion == 4) {
		switch (eventType) {
		case vrwalkinplace::ButtonEventType::ButtonPressed:
			//IVRServerDriverHost004Hooks::trackedDeviceButtonPressedOrig(m_deviceDriverHostPtr, m_openvrId, eButtonId, eventTimeOffset);
			break;
		case vrwalkinplace::ButtonEventType::ButtonUnpressed:
			//IVRServerDriverHost004Hooks::trackedDeviceButtonUnpressedOrig(m_deviceDriverHostPtr, m_openvrId, eButtonId, eventTimeOffset);
			break;
		case vrwalkinplace::ButtonEventType::ButtonTouched:
			//IVRServerDriverHost004Hooks::trackedDeviceButtonTouchedOrig(m_deviceDriverHostPtr, m_openvrId, eButtonId, eventTimeOffset);
			break;
		case vrwalkinplace::ButtonEventType::ButtonUntouched:
			//IVRServerDriverHost004Hooks::trackedDeviceButtonUntouchedOrig(m_deviceDriverHostPtr, m_openvrId, eButtonId, eventTimeOffset);
			break;
		default:
			break;
		}
	} else if (m_deviceDriverInterfaceVersion == 5) {
		auto it = _ButtonIdToComponentHandleMap.find(eButtonId);
		if (it != _ButtonIdToComponentHandleMap.end()) {
			uint64_t componentHandle = 0;
			bool newValue = false;
			switch (eventType)	{
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
				vr::VRDriverInput()->UpdateBooleanComponent(componentHandle, newValue, eventTimeOffset);
				//IVRDriverInput001Hooks::updateBooleanComponentOrig(m_driverInputPtr, componentHandle, newValue, eventTimeOffset);
			}
		} else {
			LOG(WARNING) << "Device " << m_openvrId << ": No mapping from button id " << eButtonId << " to input component.";
		}
	}
}


void DeviceManipulationHandle::ll_sendAxisEvent(uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState) {
	if (m_deviceDriverInterfaceVersion == 4) {
		//IVRServerDriverHost004Hooks::trackedDeviceAxisUpdatedOrig(m_deviceDriverHostPtr, m_openvrId, unWhichAxis, axisState);
	} else if (m_deviceDriverInterfaceVersion == 5) {
		if (unWhichAxis < 5) {
			if (_AxisIdToComponentHandleMap[unWhichAxis].first == 0 && _AxisIdToComponentHandleMap[unWhichAxis].second == 0) {
				LOG(WARNING) << "Device " << m_openvrId << ": No mapping from axis id " << unWhichAxis << " to input component.";
			} else {
				if (_AxisIdToComponentHandleMap[unWhichAxis].first != 0) {
					//sendScalarComponentUpdate(m_openvrId, unWhichAxis, 0, axisState.x, 0.0);
					vr::EVRInputError eVRIError = vr::VRDriverInput()->UpdateScalarComponent(_AxisIdToComponentHandleMap[unWhichAxis].first, axisState.x, 0);
					if (eVRIError != vr::EVRInputError::VRInputError_None) {
						LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
					}
				}
				if (_AxisIdToComponentHandleMap[unWhichAxis].second != 0) {
					//sendScalarComponentUpdate(m_openvrId, unWhichAxis, 1, axisState.y, 0.0);
					vr::EVRInputError eVRIError = vr::VRDriverInput()->UpdateScalarComponent(_AxisIdToComponentHandleMap[unWhichAxis].second, axisState.y, 0);
					if (eVRIError != vr::EVRInputError::VRInputError_None) {
						LOG(INFO) << "VR INPUT ERROR: " << eVRIError;
					}
				} 
			}
		}
	}
}


void DeviceManipulationHandle::ll_sendScalarComponentUpdate(vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) {
	if (m_deviceDriverInterfaceVersion == 5) {
		//LOG(INFO) << "bbb";
		//vr::VRDriverInput()->UpdateScalarComponent(ulComponent, fNewValue, fTimeOffset);
		//LOG(INFO) << "ccc";
		IVRDriverInput001Hooks::updateScalarComponentOrig(m_driverInputPtr, ulComponent, fNewValue, fTimeOffset);
	} else if (m_deviceDriverInterfaceVersion == 4) {
		auto it = _componentHandleToAxisIdMap.find(ulComponent);
		if (it != _componentHandleToAxisIdMap.end()) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			unsigned unWhichAxis = it->second.first;
			unsigned unWhichAxisDim = it->second.second;
			if (unWhichAxis < 5) {
				vr::VRControllerAxis_t axisState;
				if (unWhichAxisDim == 0) {
					axisState.x = fNewValue;
				}
				else {
					axisState.y = fNewValue;
				}				
				IVRServerDriverHost004Hooks::trackedDeviceAxisUpdatedOrig(m_deviceDriverHostPtr, m_openvrId, unWhichAxis, axisState);
			}
		}
	}
}


bool DeviceManipulationHandle::handleButtonEvent(uint32_t& unWhichDevice, ButtonEventType eventType, vr::EVRButtonId& eButtonId, double& eventTimeOffset) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	sendButtonEvent(unWhichDevice, eventType, eButtonId, eventTimeOffset);
	return false;
}


void DeviceManipulationHandle::RunFrame() {
}

bool DeviceManipulationHandle::handleAxisUpdate(uint32_t& unWhichDevice, uint32_t& unWhichAxis, vr::VRControllerAxis_t& axisState) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);
	sendAxisEvent(unWhichDevice, unWhichAxis, axisState);
	return false;
}


bool DeviceManipulationHandle::handleBooleanComponentUpdate(vr::VRInputComponentHandle_t& ulComponent, bool& bNewValue, double& fTimeOffset) {
	LOG(DEBUG) << "DeviceManipulationHandle::handleBooleanComponentUpdate(" << ulComponent << ", " << bNewValue << ", " << fTimeOffset << ")";
	auto it = _componentHandleToButtonIdMap.find(ulComponent);
	if (it != _componentHandleToButtonIdMap.end()) {
		ButtonEventType eventType;
		if (it->second.second == 0) { // touch
			eventType = bNewValue ? ButtonEventType::ButtonTouched : ButtonEventType::ButtonUntouched;
		} else { // press
			eventType = bNewValue ? ButtonEventType::ButtonPressed : ButtonEventType::ButtonUnpressed;
		}
		return handleButtonEvent(m_openvrId, eventType, it->second.first, fTimeOffset);
	} else {
		LOG(INFO) << "No mapping from boolean component handle " << ulComponent << " to button id";
	}
	return true;
}


bool DeviceManipulationHandle::handleScalarComponentUpdate(vr::VRInputComponentHandle_t& ulComponent, float& fNewValue, double& fTimeOffset) {
	LOG(DEBUG) << "DeviceManipulationHandle::handleScalarComponentUpdate(" << ulComponent << ", " << fNewValue << ", " << fTimeOffset << ")";
	auto it = _componentHandleToAxisIdMap.find(ulComponent);
	if (it != _componentHandleToAxisIdMap.end()) {
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		unsigned unWhichAxis = it->second.first;
		unsigned unWhichAxisDim = it->second.second;
		sendScalarComponentUpdate(m_openvrId, unWhichAxis, unWhichAxisDim, ulComponent, fNewValue, fTimeOffset);
		return false;
	} else {
		LOG(INFO) << "No mapping from scalar component handle " << ulComponent << " to axis id";
	}
	return true;
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
	} else {
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
	{ "joystick", vr::k_EButton_SteamVR_Touchpad },
	{ "trigger", vr::k_EButton_SteamVR_Trigger },
};

std::map<std::string, uint32_t> _inputComponentNameToAxisId = {
	{ "trackpad", 0 },
	{ "joystick", 1 },
	{ "trigger", 2 },
};

void DeviceManipulationHandle::inputAddBooleanComponent(const char *pchName, uint64_t * pHandle) {
	std::string sg0, sg1, sg2, sg3;
	if (_matchInputComponentName(pchName, sg0, sg1, sg2, sg3)) {
		LOG(DEBUG) << "Device Component Name Segments: \"" << sg0 << "\", \"" << sg1 << "\", \"" << sg2 << "\", \"" << sg3 << "\"";
		vr::EVRButtonId buttonId;
		int buttonType = 1; // 0 .. touch, 1 ..click
		bool errorFlag = false;
		if (!sg3.empty()) {
			LOG(ERROR) << "Device input component name \"" << pchName << "\" has too many segments.";
		} else {
			if (boost::iequals(sg0, "proximity")) { // proximity sensor
				buttonId = vr::k_EButton_ProximitySensor;
			} else if (boost::iequals(sg0, "input")) { // digital button
				boost::algorithm::to_lower(sg1);
				auto it = _inputComponentNameToButtonId.find(sg1);
				if (it != _inputComponentNameToButtonId.end()) {
					buttonId = it->second;
					if (boost::iequals(sg2, "touch")) {
						buttonType = 0;
					}
				} else {
					LOG(ERROR) << "Could not map input component name \"" << pchName << "\" to a button id.";
					errorFlag = true;
				}
			} else {
				LOG(ERROR) << "Unknown first component name segment \"" << sg0 << "\".";
				errorFlag = true;
			}
		}
		if (!errorFlag) {
			_componentHandleToButtonIdMap.emplace(*pHandle, std::make_pair(buttonId, buttonType));
			if (buttonType == 0) {
				_ButtonIdToComponentHandleMap[buttonId].first = *pHandle;
			} else {
				_ButtonIdToComponentHandleMap[buttonId].second = *pHandle;
			}
			LOG(INFO) << "Mapped input component \"" << pchName << "\" to button id (" << (int)buttonId << ", " << buttonType << ")";
		}
	} else {
		LOG(ERROR) << "Could not parse input component name \"" << pchName << "\".";
	}
}

void DeviceManipulationHandle::inputAddScalarComponent(const char *pchName, uint64_t * pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) {
	std::string sg0, sg1, sg2, sg3;
	if (_matchInputComponentName(pchName, sg0, sg1, sg2, sg3)) {
		LOG(DEBUG) << "Device Component Name Segments: \"" << sg0 << "\", \"" << sg1 << "\", \"" << sg2 << "\", \"" << sg3 << "\"";
		uint32_t axisId;
		uint32_t axisDim = 0;
		bool errorFlag = false;
		if (!sg3.empty()) {
			LOG(ERROR) << "Device input component name \"" << pchName << "\" has too many segments.";
		} else {
			if (boost::iequals(sg0, "input")) { // analog input
				boost::algorithm::to_lower(sg1);
				auto it = _inputComponentNameToAxisId.find(sg1);
				if (it != _inputComponentNameToAxisId.end()) {
					axisId = it->second;
					if (boost::iequals(sg2, "x")) {
						axisDim = 0;
					} else if (boost::iequals(sg2, "y")) {
						axisDim = 1;
					}
				} else {
					LOG(ERROR) << "Could not map input component name \"" << pchName << "\" to an axis id.";
					errorFlag = true;
				}
			} else {
				LOG(ERROR) << "Unknown first component name segment \"" << sg0 << "\".";
				errorFlag = true;
			}
		}
		if (!errorFlag) {
			_componentHandleToAxisIdMap.emplace(*pHandle, std::make_pair(axisId, axisDim));
			if (axisDim == 0) {
				_AxisIdToComponentHandleMap[axisId].first = *pHandle;
			} else {
				_AxisIdToComponentHandleMap[axisId].second = *pHandle;
			}
			LOG(INFO) << "Mapped input component \"" << pchName << "\" to axis id (" << axisId << ", " << axisDim << ")" << " with handle " << pHandle;
		}
	} else {
		LOG(ERROR) << "Could not parse input component name \"" << pchName << "\".";
	}
}

void DeviceManipulationHandle::sendButtonEvent(uint32_t unWhichDevice, ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {	
	switch (eventType) {
		case ButtonEventType::ButtonPressed: {
			ll_sendButtonEvent(ButtonEventType::ButtonPressed, eButtonId, eventTimeOffset);
		} break;
		case ButtonEventType::ButtonUnpressed: {
			ll_sendButtonEvent(ButtonEventType::ButtonUnpressed, eButtonId, eventTimeOffset);
		} break;
		case ButtonEventType::ButtonTouched: {
			ll_sendButtonEvent(ButtonEventType::ButtonTouched, eButtonId, eventTimeOffset);
		} break;
		case ButtonEventType::ButtonUntouched: {
			ll_sendButtonEvent(ButtonEventType::ButtonUntouched, eButtonId, eventTimeOffset);
		} break;
		default: {
		} break;
	}
}

void DeviceManipulationHandle::sendAxisEvent(uint32_t unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState) {
	ll_sendAxisEvent(unWhichAxis, axisState);
}

void DeviceManipulationHandle::sendScalarComponentUpdate(uint32_t unWhichDevice, uint32_t unWhichAxis, uint32_t unAxisDim, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) {
	ll_sendScalarComponentUpdate(ulComponent, fNewValue, fTimeOffset);
}

void DeviceManipulationHandle::sendScalarComponentUpdate(uint32_t unWhichDevice, uint32_t unWhichAxis, uint32_t unAxisDim, float fNewValue, double fTimeOffset) {
	if (unWhichAxis < 5) {
		uint64_t componentHandle = 0;
		if (unAxisDim == 0) {
			componentHandle = _AxisIdToComponentHandleMap[unWhichAxis].first;
		} else {
			componentHandle = _AxisIdToComponentHandleMap[unWhichAxis].second;
		}
		if (componentHandle != 0) {
			sendScalarComponentUpdate(unWhichDevice, unWhichAxis, unAxisDim, componentHandle, fNewValue, fTimeOffset);
		}
	}
}



} // end namespace driver
} // end namespace vrwalkinplace
