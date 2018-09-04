#pragma once

#include <memory>
#include <queue>
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

	void openvr_buttonEvent(uint32_t unWhichDevice, vr::EVREventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset);

	void openvr_axisEvent(uint32_t unWhichDevice, vr::EVRButtonId unWhichAxis, const vr::VRControllerAxis_t& axisState);


private:

	VirtualController ovrwip_1;

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
