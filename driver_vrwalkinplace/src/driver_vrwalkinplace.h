#pragma once

#include "stdafx.h"
#include <openvr_driver.h>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <boost/variant.hpp>
#include <atomic>
#include "logging.h"
#include <vrwalkinplace_types.h>
#include "utils/DevicePropertyValueVisitor.h"
#include "com/shm/driver_ipc_shm.h"


// driver namespace
namespace vrwalkinplace {
	namespace driver {


		// forward declarations
		class CServerDriver;
		class CClientDriver;
		class CTrackedDeviceDriver;
		class CTrackedControllerDriver;


		typedef vr::EVRInitError(*_DetourTrackedDeviceActivate_t)(vr::ITrackedDeviceServerDriver*, uint32_t);
		typedef void(*_DetourTrackedDeviceAxisUpdated_t)(vr::IVRServerDriverHost*, uint32_t, uint32_t, const vr::VRControllerAxis_t&);
		typedef void(*_DetourTrackedDeviceButtonUntouched_t)(vr::IVRServerDriverHost*, uint32_t, vr::EVRButtonId, double);
		typedef void(*_DetourTrackedDeviceButtonTouched_t)(vr::IVRServerDriverHost*, uint32_t, vr::EVRButtonId, double);
		typedef void(*_DetourTrackedDeviceButtonUnpressed_t)(vr::IVRServerDriverHost*, uint32_t, vr::EVRButtonId, double);
		typedef void(*_DetourTrackedDeviceButtonPressed_t)(vr::IVRServerDriverHost*, uint32_t, vr::EVRButtonId, double);
		typedef void(*_DetourTrackedDevicePoseUpdated_t)(vr::IVRServerDriverHost*, uint32_t, const vr::DriverPose_t&, uint32_t);
		typedef bool(*_DetourTrackedDeviceAdded_t)(vr::IVRServerDriverHost*, const char*, vr::ETrackedDeviceClass, vr::ITrackedDeviceServerDriver*);


		/**
		* Implements the IVRWatchdogProvider interface.
		*
		* Its only purpose seems to be to start SteamVR by calling WatchdogWakeUp() from the IVRWatchdogHost interface.
		* Valve probably uses this interface to start SteamVR whenever a button is pressed on the controller or hmd.
		* We must implement it but currently we don't use it for anything.
		*/
		class CWatchdogProvider : public vr::IVRWatchdogProvider {
		public:
			/** initializes the driver in watchdog mode. */
			virtual vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) override;

			/** cleans up the driver right before it is unloaded */
			virtual void Cleanup() override;
		};




		// Stores manipulation information about an openvr device
		class OpenvrWalkInPlaceInfo {
		private:
			bool m_isValid = false;
			std::recursive_mutex _mutex;
			vr::ETrackedDeviceClass m_eDeviceClass = vr::TrackedDeviceClass_Invalid;
			vr::ITrackedDeviceServerDriver* m_driver = nullptr;
			vr::IVRServerDriverHost* m_driverHost = nullptr;
			uint32_t m_openvrId = vr::k_unTrackedDeviceIndexInvalid;

			vr::IVRControllerComponent* m_controllerComponent;

			int m_deviceMode = 0; // 0 .. default, 1 step detection
			bool _disconnectedMsgSend = false;

			bool m_lastDriverPoseValid = false;
			vr::DriverPose_t m_lastDriverPose;
			long long m_lastDriverPoseTime = 0;

			vr::TrackedDevicePose_t pose;

		public:
			OpenvrWalkInPlaceInfo() {}
			OpenvrWalkInPlaceInfo(vr::ITrackedDeviceServerDriver* driver, vr::ETrackedDeviceClass eDeviceClass, uint32_t openvrId, vr::IVRServerDriverHost* driverHost)
				: m_isValid(true), m_driver(driver), m_eDeviceClass(eDeviceClass), m_openvrId(openvrId), m_driverHost(driverHost) {}

			bool isValid() const { return m_isValid; }
			vr::ETrackedDeviceClass deviceClass() const { return m_eDeviceClass; }
			vr::ITrackedDeviceServerDriver* driver() const { return m_driver; }
			vr::IVRServerDriverHost* driverHost() const { return m_driverHost; }
			uint32_t openvrId() const { return m_openvrId; }
			void setOpenvrId(uint32_t id) { m_openvrId = id; }

			vr::IVRControllerComponent* controllerComponent() { return m_controllerComponent; }

			int deviceMode() const { return m_deviceMode; }
			int setDefaultMode();
			int setStepDetectionMode();

			int _disableOldMode(int newMode);

			void handleNewDevicePose(vr::IVRServerDriverHost* driver, _DetourTrackedDevicePoseUpdated_t origFunc, uint32_t& unWhichDevice, const vr::DriverPose_t& newPose);
			void handleButtonEvent(vr::IVRServerDriverHost* driver, void* origFunc, uint32_t& unWhichDevice, ButtonEventType eventType, vr::EVRButtonId eButtonId, double eventTimeOffset);
			void handleAxisEvent(vr::IVRServerDriverHost* driver, _DetourTrackedDeviceAxisUpdated_t origFunc, uint32_t& unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t& axisState);

			bool lastDriverPoseValid() { return m_lastDriverPoseValid; }
			vr::DriverPose_t& lastDriverPose() { return m_lastDriverPose; }
			long long lastDriverPoseTime() { return m_lastDriverPoseTime; }
			void setLastDriverPoseValid(bool valid) { m_lastDriverPoseValid = valid; }
			void setLastDriverPose(const vr::DriverPose_t& pose, long long time) {
				m_lastDriverPose = pose;
				m_lastDriverPoseTime = time;
				m_lastDriverPoseValid = true;
			}
		};


		/**
		* Implements the IServerTrackedDeviceProvider interface.
		*
		* Its the main entry point of the driver. It's a singleton which manages all devices owned by this driver,
		* and also handles the whole "hacking into OpenVR" stuff.
		*/
		class CServerDriver : public vr::IServerTrackedDeviceProvider {
		public:
			CServerDriver();
			virtual ~CServerDriver();

			//// from IServerTrackedDeviceProvider ////

			/** initializes the driver. This will be called before any other methods are called. */
			virtual vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) override;

			/** cleans up the driver right before it is unloaded */
			virtual void Cleanup() override;

			/** Returns the version of the ITrackedDeviceServerDriver interface used by this driver */
			virtual const char * const *GetInterfaceVersions() { return vr::k_InterfaceVersions; }

			/** Allows the driver do to some work in the main loop of the server. Call frequency seems to be around 90Hz. */
			virtual void RunFrame() override;

			/** Returns true if the driver wants to block Standby mode. */
			virtual bool ShouldBlockStandbyMode() override { return false; }

			/** Called when the system is entering Standby mode */
			virtual void EnterStandby() override {}

			/** Called when the system is leaving Standby mode */
			virtual void LeaveStandby() override {}


			//// self ////

			static CServerDriver* getInstance() { return singleton; }

			OpenvrWalkInPlaceInfo* walkInPlace_getInfo(uint32_t unWhichDevice);

			/* Step Detection */
			void enableStepDetection(bool enable);
			void setAccuracyButton(int buttonId);
			void setGameStepType(int gameType);
			void setStepIntSec(float value);
			void setHMDThreshold(vr::HmdVector3d_t value);
			void setHandJogThreshold(float value);
			void setHandRunThreshold(float value);
			void setWalkTouch(float value);
			void setJogTouch(float value);
			void setRunTouch(float value);
			void setStepPoseDetected(bool enable);
			bool isStepDetectionEnabled();
			int accuracyButtonEnabled();
			bool _applyStepPoseDetect(vr::DriverPose_t& pose, OpenvrWalkInPlaceInfo* deviceInfo);

		private:
			static CServerDriver* singleton;

			//// ipc shm related ////
			IpcShmCommunicator shmCommunicator;


			//// openvr device manipulation related ////
			std::recursive_mutex _openvrDevicesMutex;
			static std::map<vr::ITrackedDeviceServerDriver*, std::shared_ptr<OpenvrWalkInPlaceInfo>> _openvrDeviceInfos;
			static OpenvrWalkInPlaceInfo* _openvrIdToDeviceInfoMap[vr::k_unMaxTrackedDeviceCount];


			//step detection related
			bool _stepPoseDetectEnabled = false;
			bool _stepPoseDetected = false;
			int _useAccuracyButton = vr::EVRButtonId::k_EButton_Grip;
			int _gameStepType = 1;
			int _teleportUnpressed = true;
			int _hasUnTouchedStepAxis = 0;			
			float _handJogThreshold = 0.4;
			float _handRunThreshold = 1.7;
			float walkTouch = 0.6;
			float jogTouch = 0.9;
			float runTouch = 1.0;
			double _timeLastStepTaken = 0.0;
			double _timeLastTick = 0.0;
			double _stepIntegrateSteps = 0.0;
			double _jogIntegrateSteps = 0.0;
			double _runIntegrateSteps = 0.0;
			double _stepIntegrateStepLimit = 0.07;
			int _openvrDeviceStepPoseTracker[3]; //HMD and two controllers
			vr::HmdVector3d_t _avgStepPos = { 0.0, 0.0, 0.0 };
			vr::HmdVector3d_t _stdDeviation = { 0.06, 0.04, 0.03 };
			vr::HmdVector3d_t _handsPointDir = { 0.0, 0.0, 0.0 };
			vr::HmdVector3d_t _hmdThreshold = { 0.13, 0.13, 0.13 };
			vr::HmdVector3d_t _handThreshold = { 0.0, 0.0, 0.0 };


			bool isTakingStep(double * vel, vr::HmdVector3d_t threshold, double roll, double pitch);
			bool isShakingHead(double * vel, vr::HmdVector3d_t threshold);
			bool isStepingInPlace(double * pos);
			bool isJoggingStep(double * vel);
			bool isRunningStep(double * vel);

			//// function hooks related ////

			template<class T>
			struct _DetourFuncInfo {
				bool enabled = false;
				void* targetFunc = nullptr;
				T origFunc = nullptr;
			};

			static _DetourFuncInfo<_DetourTrackedDeviceAdded_t> _deviceAddedDetour;
			static bool _deviceAddedDetourFunc(vr::IVRServerDriverHost* _this, const char *pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, vr::ITrackedDeviceServerDriver *pDriver);

			static _DetourFuncInfo<_DetourTrackedDevicePoseUpdated_t> _poseUpatedDetour;
			static void _poseUpatedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize);

			static _DetourFuncInfo<_DetourTrackedDeviceButtonPressed_t> _buttonPressedDetour;
			static void _buttonPressedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset);

			static _DetourFuncInfo<_DetourTrackedDeviceButtonUnpressed_t> _buttonUnpressedDetour;
			static void _buttonUnpressedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset);

			static _DetourFuncInfo<_DetourTrackedDeviceButtonTouched_t> _buttonTouchedDetour;
			static void _buttonTouchedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset);

			static _DetourFuncInfo<_DetourTrackedDeviceButtonUntouched_t> _buttonUntouchedDetour;
			static void _buttonUntouchedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset);

			static _DetourFuncInfo<_DetourTrackedDeviceAxisUpdated_t> _axisUpdatedDetour;
			static void _axisUpdatedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t & axisState);

			static std::vector<_DetourFuncInfo<_DetourTrackedDeviceActivate_t>> _deviceActivateDetours;
			static vr::EVRInitError _deviceActivateDetourFunc(vr::ITrackedDeviceServerDriver* _this, uint32_t unObjectId);
			static std::map<vr::ITrackedDeviceServerDriver*, _DetourFuncInfo<_DetourTrackedDeviceActivate_t>*> _deviceActivateDetourMap; // _this => DetourInfo

		};



		/**
		* Implements the ITrackedDeviceServerDriver interface.
		*
		* Represents a single virtual device managed by this driver. It has a serial number, a device type, device properties and a pose.
		**/
		class CTrackedDeviceDriver : public vr::ITrackedDeviceServerDriver {
		protected:
			std::recursive_mutex _mutex;

			CServerDriver* m_serverDriver;
			VirtualDeviceType m_deviceType;
			std::string m_serialNumber;
			bool m_published = false;
			bool m_periodicPoseUpdates = true;
			uint32_t m_openvrId = vr::k_unTrackedDeviceIndexInvalid;
			uint32_t m_virtualDeviceId;
			vr::PropertyContainerHandle_t m_propertyContainer = vr::k_ulInvalidPropertyContainer;

			vr::DriverPose_t m_pose;
			typedef boost::variant<int32_t, uint64_t, float, bool, std::string, vr::HmdMatrix34_t, vr::HmdMatrix44_t, vr::HmdVector3_t, vr::HmdVector4_t> _devicePropertyType_t;
			std::map<int, _devicePropertyType_t> _deviceProperties;

		public:
			CTrackedDeviceDriver(CServerDriver* parent, VirtualDeviceType type, const std::string& serial, uint32_t virtualId = vr::k_unTrackedDeviceIndexInvalid);

			// from ITrackedDeviceServerDriver

			virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
			virtual void Deactivate() override;
			virtual void EnterStandby() override {}
			virtual void *GetComponent(const char *pchComponentNameAndVersion) override;
			virtual void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override {}
			virtual vr::DriverPose_t GetPose() override;

			// from self

			const std::string& serialNumber() { return m_serialNumber; }
			VirtualDeviceType deviceType() { return m_deviceType; }
			bool published() { return m_published; }
			bool periodicPoseUpdates() { return m_periodicPoseUpdates; }

			CServerDriver* serverDriver() { return m_serverDriver; }
			uint32_t openvrDeviceId() { return m_openvrId; }
			uint32_t virtualDeviceId() { return m_virtualDeviceId; }

			vr::DriverPose_t& driverPose() { return m_pose; }

			bool enablePeriodicPoseUpdates(bool enabled) { return m_periodicPoseUpdates; }
			void publish();

			void updatePose(const vr::DriverPose_t& newPose, double timeOffset, bool notify = true);
			void sendPoseUpdate(double timeOffset = 0.0, bool onlyWhenConnected = true);

			template<class T>
			T getTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, vr::ETrackedPropertyError * pError) {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				if (pError) {
					*pError = vr::TrackedProp_Success;
				}
				auto p = _deviceProperties.find((int)prop);
				if (p != _deviceProperties.end()) {
					try {
						T retval = boost::get<T>(p->second);
						return retval;
					}
					catch (std::exception&) {
						if (pError) {
							*pError = vr::TrackedProp_WrongDataType;
						}
						return T();
					}
				}
				else {
					if (pError) {
						*pError = vr::TrackedProp_ValueNotProvidedByDevice;
					}
					return T();
				}
			}

			template<class T>
			void setTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, const T& value, bool notify = true) {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				_deviceProperties[prop] = value;
				if (notify && m_openvrId != vr::k_unTrackedDeviceIndexInvalid) {
					auto errorMessage = boost::apply_visitor(DevicePropertyValueVisitor(m_propertyContainer, prop), _deviceProperties[prop]);
					if (!errorMessage.empty()) {
						LOG(ERROR) << "Could not set tracked device property: " << errorMessage;
					}
				}
			}

			void removeTrackedDeviceProperty(vr::ETrackedDeviceProperty prop, bool notify = true) {
				std::lock_guard<std::recursive_mutex> lock(_mutex);
				auto i = _deviceProperties.find(prop);
				if (i != _deviceProperties.end()) {
					_deviceProperties.erase(i);
					if (notify && m_openvrId != vr::k_unTrackedDeviceIndexInvalid) {
						vr::VRProperties()->EraseProperty(m_propertyContainer, prop);
					}
				}
			}
		};



		/**
		* Extends CTrackedDeviceDriver by implementing the IVRControllerComponent component.
		*
		* Represents a single virtual motion controller managed by this driver. Adds a controller state the the virtual device.
		**/
		class CTrackedControllerDriver : public CTrackedDeviceDriver, public vr::IVRControllerComponent {
		private:
			vr::VRControllerState_t m_ControllerState;

		public:
			CTrackedControllerDriver(CServerDriver* parent, const std::string& serial);

			// from ITrackedDeviceServerDriver

			virtual void *GetComponent(const char *pchComponentNameAndVersion) override;

			// from IVRControllerComponent

			virtual vr::VRControllerState_t GetControllerState() override;

			// from self

			vr::VRControllerState_t& controllerState() { return m_ControllerState; }

			void updateControllerState(const vr::VRControllerState_t& newState, double timeOffset, bool notify = true);
			void buttonEvent(ButtonEventType eventType, uint32_t buttonId, double timeOffset, bool notify = true);
			void axisEvent(uint32_t axisId, const vr::VRControllerAxis_t& axisState, bool notify = true);

		};

	} // end namespace driver

} // end namespace vrwalkinplace
