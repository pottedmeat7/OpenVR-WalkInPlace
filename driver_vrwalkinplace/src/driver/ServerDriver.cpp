
#include "ServerDriver.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace vrwalkinplace {
	namespace driver {

		const char* const ServerDriver::interfaces_[] = {
			vr::IServerTrackedDeviceProvider_Version,
			vr::IVRWatchdogProvider_Version
		};

		std::string ServerDriver::installDir;

		ServerDriver::ServerDriver() {
		}

		ServerDriver::~ServerDriver() {
			LOG(TRACE) << "CServerDriver::~CServerDriver_VRWalkInPlace()";
		}

		vr::EVRInitError ServerDriver::Init(vr::IVRDriverContext *pDriverContext) {
			LOG(TRACE) << "CServerDriver::Init()";

			LOG(DEBUG) << "Initialize driver context.";
			VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

			// Read installation directory
			vr::ETrackedPropertyError tpeError;
			installDir = vr::VRProperties()->GetStringProperty(pDriverContext->GetDriverHandle(), vr::Prop_InstallPath_String, &tpeError);
			if (tpeError == vr::TrackedProp_Success) {
				LOG(INFO) << "Install Dir:" << installDir;
			}
			else {
				LOG(INFO) << "Could not get Install Dir: " << vr::VRPropertiesRaw()->GetPropErrorNameFromEnum(tpeError);
			}

			// Start IPC thread
			shmCommunicator.init(this);

			return vr::VRInitError_None;
		}


		void ServerDriver::Cleanup() {
			LOG(TRACE) << "CServerDriver::Cleanup()";
			shmCommunicator.shutdown();
			//VR_CLEANUP_SERVER_DRIVER_CONTEXT();
		}


		// Call frequency: ~93Hz
		void ServerDriver::RunFrame() {
		}

		void ServerDriver::openvr_deviceAdded(uint32_t unWhichDevice, bool leftRole) {
			LOG(TRACE) << "CServerDriver::Added New Virtual Controller";
			if (hmdID == vr::k_unTrackedDeviceIndexInvalid) {
				hmdID = unWhichDevice;
			}
		}

		void ServerDriver::openvr_poseUpdate(uint32_t unWhichDevice, const vr::DriverPose_t & pose, double eventTimeOffset) {
			if (initDriver) {
				vr::DriverPose_t devicePose = pose;
				devicePose.qWorldFromDriverRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[hmdID].mDeviceToAbsoluteTracking);
				devicePose.vecWorldFromDriverTranslation[0] = latestDevicePoses[hmdID].mDeviceToAbsoluteTracking.m[0][3];
				devicePose.vecWorldFromDriverTranslation[1] = latestDevicePoses[hmdID].mDeviceToAbsoluteTracking.m[1][3];
				devicePose.vecWorldFromDriverTranslation[2] = latestDevicePoses[hmdID].mDeviceToAbsoluteTracking.m[2][3];
				vr_locomotion1.updatePose(devicePose);
				vr::VRServerDriverHost()->TrackedDevicePoseUpdated(vr_locomotion1.openvrId(), vr_locomotion1.GetPose(), sizeof(vr::DriverPose_t));
			}
		}

		void ServerDriver::openvr_updateState(uint32_t unWhichDevice, vr::VRControllerState_t new_state, double eventTimeOffset) {			 
			if (initDriver) {
				vr_locomotion1.updateState(new_state);
			}
		}

		void ServerDriver::openvr_buttonEvent(uint32_t unWhichDevice, ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			if (initDriver) {
				vr_locomotion1.sendButtonEvent(eventType, eButtonId, eventTimeOffset);
			}
		}

		void ServerDriver::openvr_axisEvent(uint32_t unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t & axisState) {
			if (initDriver) {
				vr_locomotion1.sendAxisEvent(unWhichAxis, axisState);
			}
		}

		void ServerDriver::openvr_enableDriver(bool val) {
			if (!initDriver && val) {
				initDriver = true;

				vr::DriverPose_t test_pose = { 0 };
				test_pose.deviceIsConnected = true;
				test_pose.poseIsValid = true;
				test_pose.willDriftInYaw = false;
				test_pose.shouldApplyHeadModel = false;
				test_pose.poseTimeOffset = 0;
				test_pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
				test_pose.qDriverFromHeadRotation = { 1,0,0,0 };
				test_pose.qWorldFromDriverRotation = { 1,0,0,0 };

				vr::VRControllerState_t test_state;
				test_state.ulButtonPressed = test_state.ulButtonTouched = 0;

				vr_locomotion1 = VirtualController("ovrwip_", true, test_pose, test_state);

				vr::VRServerDriverHost()->TrackedDeviceAdded("ovrwip_", vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, &vr_locomotion1);

			}
		}

		void ServerDriver::reActivateLocomotionController(bool leftMode) {

		}

		const char * const * ServerDriver::GetInterfaceVersions()
		{
			return vr::k_InterfaceVersions;
			//return interfaces_;
		}

		bool ServerDriver::ShouldBlockStandbyMode()
		{
			return false;
		}

		void ServerDriver::EnterStandby()
		{
		}

		void ServerDriver::LeaveStandby()
		{
		}

	} // end namespace driver
} // end namespace vrwalkinplace
