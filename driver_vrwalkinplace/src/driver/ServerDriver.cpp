
#include "ServerDriver.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <cstdlib>
#include <iostream>

#define LOG(text) std::cerr << text << std::endl;

constexpr char logConfigFileName[] = "logging.conf";

constexpr char logConfigDefault[] =
"* GLOBAL:\n"
"	FORMAT = \"[%level] %datetime{%Y-%M-%d %H:%m:%s}: %msg\"\n"
"	FILENAME = \"driver_vrwalkinplace.log\"\n"
"	ENABLED = true\n"
"	TO_FILE = true\n"
"	TO_STANDARD_OUTPUT = true\n"
"	MAX_LOG_FILE_SIZE = 2097152 ## 2MB\n"
"* TRACE:\n"
"	ENABLED = true\n"
"* DEBUG:\n"
"	ENABLED = true\n";

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#else
#endif

void init_logging_linux() {
	if (isWindows){
		//do nothing
	} else {
	}
}


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
			LOG("CServerDriver::~CServerDriver_VRWalkInPlace()");
		}

		vr::EVRInitError ServerDriver::Init(vr::IVRDriverContext *pDriverContext) {
			init_logging_linux();
			LOG("CServerDriver::Init()");

			LOG("Initialize driver context.");
			VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

			// Read installation directory
			vr::ETrackedPropertyError tpeError;
			installDir = vr::VRProperties()->GetStringProperty(pDriverContext->GetDriverHandle(), vr::Prop_InstallPath_String, &tpeError);
			if (tpeError == vr::TrackedProp_Success) {
				LOG("Install Dir:" << installDir);
			}
			else {
				LOG("Could not get Install Dir: " << vr::VRPropertiesRaw()->GetPropErrorNameFromEnum(tpeError));
			}

			// Start IPC thread
			shmCommunicator.init(this);

			return vr::VRInitError_None;
		}


		void ServerDriver::Cleanup() {
			LOG("CServerDriver::Cleanup()");
			shmCommunicator.shutdown();
			VR_CLEANUP_SERVER_DRIVER_CONTEXT();
		}


		// Call frequency: ~93Hz
		void ServerDriver::RunFrame() {
		}

		void ServerDriver::openvr_hmdAdded(uint32_t unWhichDevice) {
			if (hmdID == vr::k_unTrackedDeviceIndexInvalid) {
				hmdID = unWhichDevice;
			}
		}

		void ServerDriver::openvr_followDevice(uint32_t unWhichDevice) {
			followDevice = unWhichDevice;
		}

		void ServerDriver::openvr_poseUpdate(uint32_t unWhichDevice, const vr::DriverPose_t & pose, double eventTimeOffset) {
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
			try {
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

					vr_locomotion1 = VirtualController("ovrwip_000", true, test_pose, test_state);

					vr::VRServerDriverHost()->TrackedDeviceAdded("ovrwip_000", vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, &vr_locomotion1);

				}
			}
			catch (std::exception& e) {
				initDriver = false;
				LOG("Exception caught while enabling driver: " << e.what());
			}
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
