
#include "stdafx.h"
#include "driver_vrwalkinplace.h"
#include <openvr_math.h>


namespace vrwalkinplace {
	namespace driver {


#define VECTOR_ADD(lhs, rhs) \
		lhs[0] += rhs.v[0]; \
		lhs[1] += rhs.v[1]; \
		lhs[2] += rhs.v[2];


		void OpenvrDeviceManipulationInfo::handleNewDevicePose(vr::IVRServerDriverHost* driver, _DetourTrackedDevicePoseUpdated_t origFunc, uint32_t& unWhichDevice, const vr::DriverPose_t& pose) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			vr::DriverPose_t newPose = pose;
			auto serverDriver = CServerDriver::getInstance();
			if (serverDriver) {
				if (serverDriver->isStepDetectionEnabled()) {
					//LOG(INFO) << "server driver is step detection enabled";
					if (m_deviceRotationOffset.w != 1.0 || m_deviceRotationOffset.x != 0.0
						|| m_deviceRotationOffset.y != 0.0 || m_deviceRotationOffset.z != 0.0) {
						serverDriver->_applyStepPoseDetect(newPose, this, m_deviceRotationOffset);
					}
					else {
						vr::HmdQuaternion_t stepUpDir = { 1.0, 0.0, 0.0, 0.0 };
						serverDriver->_applyStepPoseDetect(newPose, this, stepUpDir);
					}
				}
			}
			origFunc(driver, unWhichDevice, newPose, sizeof(vr::DriverPose_t));
		}


		void OpenvrDeviceManipulationInfo::handleButtonEvent(vr::IVRServerDriverHost* driver, void* origFunc, uint32_t& unWhichDevice, ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			auto serverDriver = CServerDriver::getInstance();
			if (serverDriver) {
				if (!serverDriver->isStepDetectionEnabled() || (eButtonId != vr::k_EButton_A && eventType != ButtonEventType::ButtonTouched && eventType != ButtonEventType::ButtonUntouched)) {
					vr::EVRButtonId button = eButtonId;
					getButtonMapping(eButtonId, button);
					((_DetourTrackedDeviceButtonPressed_t)origFunc)(driver, unWhichDevice, button, eventTimeOffset);
				}
			}
			else {
				vr::EVRButtonId button = eButtonId;
				getButtonMapping(eButtonId, button);
				((_DetourTrackedDeviceButtonPressed_t)origFunc)(driver, unWhichDevice, button, eventTimeOffset);
			}
		}

		void OpenvrDeviceManipulationInfo::handleAxisEvent(vr::IVRServerDriverHost* driver, _DetourTrackedDeviceAxisUpdated_t origFunc, uint32_t& unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			auto serverDriver = CServerDriver::getInstance();
			if (serverDriver) {
				if (!serverDriver->isStepDetectionEnabled() || unWhichAxis != vr::k_EButton_Axis0) {
					origFunc(driver, unWhichDevice, unWhichAxis, axisState);
				}
			}
			else {
				origFunc(driver, unWhichDevice, unWhichAxis, axisState);
			}
		}


		bool OpenvrDeviceManipulationInfo::triggerHapticPulse(uint32_t unAxisId, uint16_t usPulseDurationMicroseconds, bool directMode) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (m_controllerComponent) {
				return m_triggerHapticPulseFunc(m_controllerComponent, unAxisId, usPulseDurationMicroseconds);
			}
			return true;
		}

		bool OpenvrDeviceManipulationInfo::getButtonMapping(vr::EVRButtonId button, vr::EVRButtonId& mappedButton) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (m_enableButtonMapping) {
				auto i = m_buttonMapping.find(button);
				if (i != m_buttonMapping.end()) {
					mappedButton = i->second;
					return true;
				}
			}
			return false;
		}

		void OpenvrDeviceManipulationInfo::addButtonMapping(vr::EVRButtonId button, vr::EVRButtonId mappedButton) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			m_buttonMapping[button] = mappedButton;
		}

		void OpenvrDeviceManipulationInfo::eraseButtonMapping(vr::EVRButtonId button) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			m_buttonMapping.erase(button);
		}

		void OpenvrDeviceManipulationInfo::eraseAllButtonMappings() {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			m_buttonMapping.clear();
		}

		int OpenvrDeviceManipulationInfo::setDefaultMode() {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			auto res = _disableOldMode(0);
			if (res == 0) {
				m_deviceMode = 0;
			}
			return 0;
		}

		int OpenvrDeviceManipulationInfo::setStepDetectionMode() {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			auto res = _disableOldMode(6);
			auto serverDriver = CServerDriver::getInstance();
			if (res == 0 && serverDriver) {
				_disconnectedMsgSend = false;
				m_deviceMode = 6;
			}
			return 0;
		}

		int OpenvrDeviceManipulationInfo::_disableOldMode(int newMode) {
			return 0;
		}


		void OpenvrDeviceManipulationInfo::setControllerComponent(vr::IVRControllerComponent* component, _DetourTriggerHapticPulse_t triggerHapticPulse) {
			m_controllerComponent = component;
			m_triggerHapticPulseFunc = triggerHapticPulse;
		}

	} // end namespace driver
} // end namespace vrwalkinplace
