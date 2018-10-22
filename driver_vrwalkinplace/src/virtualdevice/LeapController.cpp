#include "LeapController.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "../driver/ServerDriver.h"

namespace vrwalkinplace {
	namespace driver {

		LeapController::LeapController() {

		}

		LeapController::LeapController(const LeapController &handle) {

		}

		LeapController::LeapController(std::string serial, bool side, vr::DriverPose_t initial_pose, vr::VRControllerState_t initial_state) :
			VirtualController(serial, side, initial_pose, initial_state) {
			leapControl.addListener(*this);
		}

		LeapController::~LeapController() {
		}

		void LeapController::Deactivate()
		{
			leapControl.removeListener(*this);
		}

		void LeapController::RunFrame(vr::DriverPose_t hmd_pose) {

			hmdPose = hmd_pose;

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_openvrId, devicePose, sizeof(vr::DriverPose_t));
		}

		void LeapController::RunFrame() {

			vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_openvrId, devicePose, sizeof(vr::DriverPose_t));
		}

		void LeapController::onConnect(const Leap::Controller& controller) {
			LOG(INFO) << "Connected" << std::endl;
		}

		void LeapController::onFrame(const Leap::Controller& controller) {
			Leap::Frame frame0 = controller.frame();
			LOG(INFO) << "Frame id: " << frame0.id()
				<< ", timestamp: " << frame0.timestamp()
				<< ", hands: " << frame0.hands().count()
				<< ", fingers: " << frame0.fingers().count();

			vr::DriverPose_t driverPose;
			auto timeOffset = ((double)frame0.timestamp() - controller.now()) / 1000000.0;
			driverPose.poseTimeOffset = timeOffset;
			devicePose.deviceIsConnected = true;

			if (true || hmdPose.poseIsValid) {

				//leftPose.qWorldFromDriverRotation = leftRotationOffset * vrmath::quaternionFromRotationMatrix(hmdPose.mDeviceToAbsoluteTracking);
				//devicePose.vecWorldFromDriverTranslation[0] = hmdPose.vecWorldFromDriverTranslation[0];
				//devicePose.vecWorldFromDriverTranslation[1] = hmdPose.vecWorldFromDriverTranslation[1];
				//devicePose.vecWorldFromDriverTranslation[2] = hmdPose.vecWorldFromDriverTranslation[2];


				for (auto& h : frame0.hands()) {
					if (h.isValid()) {
						if ((h.isLeft() && leftSide) || (h.isRight() && !leftSide)) {
							Leap::Vector position = h.palmPosition();
							Leap::Vector velocity = h.palmVelocity();

							devicePose.vecPosition[0] = -0.001*position.x;
							devicePose.vecPosition[1] = -0.001*position.z;
							devicePose.vecPosition[2] = -0.001*position.y;

							devicePose.vecVelocity[0] = -0.001*velocity.x;
							devicePose.vecVelocity[1] = -0.001*velocity.z;
							devicePose.vecVelocity[2] = -0.001*velocity.y;

							Leap::Vector d = { -h.direction().x, -h.direction().z, -h.direction().y };
							Leap::Vector palmNormal = { -h.palmNormal().x, -h.palmNormal().z, -h.palmNormal().y };
							float yaw = d.yaw();
							float pitch = d.pitch();
							float roll = palmNormal.roll();
							devicePose.qRotation = vrmath::quaternionFromYawPitchRoll(-yaw, pitch, roll);

							devicePose.poseIsValid = true;
							devicePose.result = vr::ETrackingResult::TrackingResult_Running_OK;

							double fingerBendFactor[5];
							std::map<Leap::Finger::Type, Leap::Finger> fingerMap;
							for (auto& f : h.fingers()) {
								fingerMap[f.type()] = f;
							}
							Leap::Vector pd;
							for (int i = 0; i < 4; ++i) {
								auto& b = fingerMap[Leap::Finger::Type::TYPE_RING].bone((Leap::Bone::Type)i);
								Leap::Vector d = -b.direction();
								if (i > 0) {
									auto a = d.angleTo(pd);
									fingerBendFactor[(int)Leap::Finger::Type::TYPE_RING] += a;
								}
								pd = d;
							}
							pd.zero();
							for (int i = 0; i < 4; ++i) {
								auto& b = fingerMap[Leap::Finger::Type::TYPE_MIDDLE].bone((Leap::Bone::Type)i);
								Leap::Vector d = -b.direction();
								if (i > 0) {
									auto a = d.angleTo(pd);
									fingerBendFactor[(int)Leap::Finger::Type::TYPE_MIDDLE] += a;
								}
								pd = d;
							}
							pd.zero();
							for (int i = 0; i < 4; ++i) {
								auto& b = fingerMap[Leap::Finger::Type::TYPE_INDEX].bone((Leap::Bone::Type)i);
								Leap::Vector d = -b.direction();
								if (i > 0) {
									auto a = d.angleTo(pd);
									fingerBendFactor[(int)Leap::Finger::Type::TYPE_INDEX] += a;
								}
								pd = d;
							}
							pd.zero();
							for (int i = 0; i < 4; ++i) {
								auto& b = fingerMap[Leap::Finger::Type::TYPE_THUMB].bone((Leap::Bone::Type)i);
								Leap::Vector d = -b.direction();
								if (i > 0) {
									auto a = d.angleTo(pd);
									fingerBendFactor[(int)Leap::Finger::Type::TYPE_THUMB] += a;
								}
								pd = d;
							}
							if (h.isRight()) {
								if (fingerBendFactor[(int)Leap::Finger::Type::TYPE_INDEX] > 0.4) {
									if (fingerBendFactor[(int)Leap::Finger::Type::TYPE_THUMB] < 0.2) {
										vr::VRControllerAxis_t axisState;
										axisState.x = (float)((fingerBendFactor[0] - 0.4) / 0.6);
										axisState.y = 0;
										sendAxisEvent(m_openvrId, axisState);
										if (axisState.x >= 0.95) {
											sendButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, vr::EVRButtonId::k_EButton_SteamVR_Trigger, 0);
										}
										else {
											sendButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, vr::EVRButtonId::k_EButton_SteamVR_Trigger, 0);
										}
									}
									else if (fingerBendFactor[(int)Leap::Finger::Type::TYPE_MIDDLE] > 0.4) {
										sendButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, vr::EVRButtonId::k_EButton_Grip, 0);
									}
								}
								else if (fingerBendFactor[(int)Leap::Finger::Type::TYPE_MIDDLE] > 0.4 && fingerBendFactor[(int)Leap::Finger::Type::TYPE_RING] > 0.4) {
									sendButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, vr::EVRButtonId::k_EButton_ApplicationMenu, 0);
								}
							}
						}
					}
				}
			}

		}

	} // end namespace driver
} // end namespace vrwalkinplace
