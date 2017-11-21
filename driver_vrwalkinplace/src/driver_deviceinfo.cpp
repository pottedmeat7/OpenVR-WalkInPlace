
#include "stdafx.h"
#include "driver_vrwalkinplace.h"
#include <openvr_math.h>


namespace vrwalkinplace {
	namespace driver {


#define VECTOR_ADD(lhs, rhs) \
		lhs[0] += rhs.v[0]; \
		lhs[1] += rhs.v[1]; \
		lhs[2] += rhs.v[2];


		void OpenvrWalkInPlaceInfo::handleNewDevicePose(vr::IVRServerDriverHost* driver, _DetourTrackedDevicePoseUpdated_t origFunc, uint32_t& unWhichDevice, const vr::DriverPose_t& pose) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			vr::DriverPose_t newPose = pose;
			auto serverDriver = CServerDriver::getInstance();
			if (serverDriver) {
				serverDriver->_applyStepPoseDetect(newPose, this);
			}
			origFunc(driver, unWhichDevice, newPose, sizeof(vr::DriverPose_t));
		}


		void OpenvrWalkInPlaceInfo::handleButtonEvent(vr::IVRServerDriverHost* driver, void* origFunc, uint32_t& unWhichDevice, ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (false) {
				auto serverDriver = CServerDriver::getInstance();
				if (serverDriver) {
					((_DetourTrackedDeviceButtonPressed_t)origFunc)(driver, unWhichDevice, eButtonId, eventTimeOffset);
				}
			}
			else {
				((_DetourTrackedDeviceButtonPressed_t)origFunc)(driver, unWhichDevice, eButtonId, eventTimeOffset);
			}
		}

		void OpenvrWalkInPlaceInfo::handleAxisEvent(vr::IVRServerDriverHost* driver, _DetourTrackedDeviceAxisUpdated_t origFunc, uint32_t& unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState) {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (false) {
				auto serverDriver = CServerDriver::getInstance();
				if (serverDriver) {
					origFunc(driver, unWhichDevice, unWhichAxis, axisState);
				}
			}
			else {
				origFunc(driver, unWhichDevice, unWhichAxis, axisState);
			}
		}

		int OpenvrWalkInPlaceInfo::setDefaultMode() {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			auto res = _disableOldMode(0);
			if (res == 0) {
				m_deviceMode = 0;
			}
			return 0;
		}

		int OpenvrWalkInPlaceInfo::setStepDetectionMode() {
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			auto res = _disableOldMode(6);
			auto serverDriver = CServerDriver::getInstance();
			if (res == 0 && serverDriver) {
				_disconnectedMsgSend = false;
				m_deviceMode = 1;
			}
			return 0;
		}

		int OpenvrWalkInPlaceInfo::_disableOldMode(int newMode) {
			return 0;
		}

	} // end namespace driver
} // end namespace vrwalkinplace
