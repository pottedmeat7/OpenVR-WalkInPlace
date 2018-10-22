#pragma once
#pragma once

#include <openvr_driver.h>
#include <vrwalkinplace_types.h>
#include <openvr_math.h>
#include "../logging.h"
#include "VirtualController.h"
#include "Leap.h"


// driver namespace
namespace vrwalkinplace {
	namespace driver {

		// Stores manipulation information about an openvr device
		class LeapController : public VirtualController, public Leap::Listener {
		private:
			Leap::Controller leapControl;
			vr::DriverPose_t hmdPose;
		public:
			LeapController();
			LeapController(const LeapController &handle);
			~LeapController();
			LeapController(std::string serial, bool side, vr::DriverPose_t initial_pose, vr::VRControllerState_t initial_state);

			virtual void Deactivate() override;

			void RunFrame(vr::DriverPose_t hmd_pose);
			void RunFrame();

			virtual void onConnect(const Leap::Controller&);
			virtual void onFrame(const Leap::Controller&);

		};


	} // end namespace driver
} // end namespace vrwalkinplace
