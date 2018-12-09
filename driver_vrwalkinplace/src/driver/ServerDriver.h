#pragma once

#include <memory>
#include <queue>
#include <vector>
#include <openvr_driver.h>
#include <vrwalkinplace_types.h>
#include <openvr_math.h>
#include "../logging.h"
#include "../com/shm/driver_ipc_shm.h"
#include "../virtualdevice/VirtualController.h";


// driver namespace
namespace vrwalkinplace {
namespace driver {

/**
* Implements the IServerTrackedDeviceProvider interface.
*
* Its the main entry point of the driver. It's a singleton which manages all devices owned by this driver, 
* and also handles the whole "hacking into OpenVR" stuff.
*/
class ServerDriver : public vr::IServerTrackedDeviceProvider {
public:
	ServerDriver();
	~ServerDriver();

	//// from IServerTrackedDeviceProvider ////

	/** initializes the driver. This will be called before any other methods are called. */
	vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) override;

	/** cleans up the driver right before it is unloaded */
	void Cleanup() override;

	/** Returns the version of the ITrackedDeviceServerDriver interface used by this driver */
	const char * const *GetInterfaceVersions();

	/** Allows the driver do to some work in the main loop of the server. Call frequency seems to be around 90Hz. */
	void RunFrame() override;

	/** Returns true if the driver wants to block Standby mode. */
	bool ShouldBlockStandbyMode() override;

	/** Called when the system is entering Standby mode */
	void EnterStandby() override;

	/** Called when the system is leaving Standby mode */
	void LeaveStandby() override;


	//// self ////

	static std::string getInstallDirectory() { return installDir; }

	void openvr_hmdAdded(uint32_t unWhichDevice);
	void openvr_followDevice(uint32_t unWhichDevice);
	void openvr_poseUpdate(uint32_t unWhichDevice, const vr::DriverPose_t & pose, double eventTimeOffset);
	void openvr_updateState(uint32_t unWhichDevice, vr::VRControllerState_t new_state, double eventTimeOffset);
	void openvr_buttonEvent(uint32_t unWhichDevice, ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset);
	void openvr_axisEvent(uint32_t unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState);
	void openvr_enableDriver(bool val);

private:

	VirtualController vr_locomotion1;
	uint64_t followDevice = vr::k_unTrackedDeviceIndexInvalid;
	uint64_t hmdID = vr::k_unTrackedDeviceIndexInvalid;
	bool initDriver = false;

	std::map<uint32_t,VirtualController> _openvrIdToVirtualControllerMap;

	vr::TrackedDevicePose_t latestDevicePoses[vr::k_unMaxTrackedDeviceCount];

	static const char* const interfaces_[];

	bool loadedInit = false;

	static std::string installDir;

	//// ipc shm related ////
	IpcShmCommunicator shmCommunicator;

	//// device manipulation related ////
	std::vector<VirtualController> _virtualControllers;

};


} // end namespace driver
} // end namespace vrwalkinplace
