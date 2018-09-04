
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

			ovrwip_1 = VirtualController("ovrwip_1", true, test_pose, test_state);

			vr::VRServerDriverHost()->TrackedDeviceAdded("ovrwip_1", vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, &ovrwip_1);

			//LOG(INFO) << "Successfully added device " << ovrwip_1.serialNumber() << " (OpenVR Id: " << unObjectId << ") (" << ovrwip_1.openvrId() << ")";

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
			/*for (auto d : _virtualControllerHandles) {
				d.second->RunFrame();
			}*/
		}

		void ServerDriver::openvr_buttonEvent(uint32_t unWhichDevice, vr::EVREventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			//_openvrIdToVirtualControllerMap[unWhichDevice]->sendButtonEvent(eventType, eButtonId, eventTimeOffset);
			ovrwip_1.sendButtonEvent(eventType, eButtonId, eventTimeOffset);
		}

		void ServerDriver::openvr_axisEvent(uint32_t unWhichDevice, vr::EVRButtonId unWhichAxis, const vr::VRControllerAxis_t & axisState) {
			//_openvrIdToVirtualControllerMap[unWhichDevice]->sendAxisEvent(unWhichAxis, axisState);
			ovrwip_1.sendAxisEvent(unWhichAxis, axisState);
		}

		const char * const * ServerDriver::GetInterfaceVersions()
		{
			return interfaces_;
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
