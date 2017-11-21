
#include "stdafx.h"
#include "driver_vrwalkinplace.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/lexical_cast.hpp>
#include <ipc_protocol.h>
#include <openvr_math.h>
#include <MinHook.h>
#include <map>
#include <vector>


namespace vrwalkinplace {
	namespace driver {

# define M_PI           3.14159265358979323846

#define CREATE_MH_HOOK(detourInfo, detourFunc, logName, objPtr, vtableOffset) \
	detourInfo.targetFunc = (*((void***)objPtr))[vtableOffset]; \
	mhError = MH_CreateHook(detourInfo.targetFunc, (void*)&detourFunc, reinterpret_cast<LPVOID*>(&detourInfo.origFunc)); \
	if (mhError == MH_OK) { \
		mhError = MH_EnableHook(detourInfo.targetFunc); \
		if (mhError == MH_OK) { \
			detourInfo.enabled = true; \
			LOG(INFO) << logName << " hook is enabled"; \
		} else { \
			MH_RemoveHook(detourInfo.targetFunc); \
			LOG(ERROR) << "Error while enabling " << logName << " hook: " << MH_StatusToString(mhError); \
		} \
	} else { \
		LOG(ERROR) << "Error while creating " << logName << " hook: " << MH_StatusToString(mhError); \
	}


#define REMOVE_MH_HOOK(detourInfo) \
	if (detourInfo.enabled) { \
		MH_RemoveHook(detourInfo.targetFunc); \
		detourInfo.enabled = false; \
	}

		CServerDriver* CServerDriver::singleton = nullptr;

		std::map<vr::ITrackedDeviceServerDriver*, std::shared_ptr<OpenvrWalkInPlaceInfo>> CServerDriver::_openvrDeviceInfos;
		OpenvrWalkInPlaceInfo* CServerDriver::_openvrIdToDeviceInfoMap[vr::k_unMaxTrackedDeviceCount]; // index == openvrId

		bool g_stepDetectEnabled = false;
		bool g_isHoldingAccuracyButton = false;
		bool g_stepPoseDetected = false;
		bool g_jogPoseDetected = false;
		bool g_runPoseDetected = false;

		CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceAdded_t> CServerDriver::_deviceAddedDetour;
		CServerDriver::_DetourFuncInfo<_DetourTrackedDevicePoseUpdated_t> CServerDriver::_poseUpatedDetour;
		CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceButtonPressed_t> CServerDriver::_buttonPressedDetour;
		CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceButtonUnpressed_t> CServerDriver::_buttonUnpressedDetour;
		CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceButtonTouched_t> CServerDriver::_buttonTouchedDetour;
		CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceButtonUntouched_t> CServerDriver::_buttonUntouchedDetour;
		CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceAxisUpdated_t> CServerDriver::_axisUpdatedDetour;
		std::vector<CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceActivate_t>> CServerDriver::_deviceActivateDetours;
		std::map<vr::ITrackedDeviceServerDriver*, CServerDriver::_DetourFuncInfo<_DetourTrackedDeviceActivate_t>*> CServerDriver::_deviceActivateDetourMap; // _this => DetourInfo

		CServerDriver::CServerDriver() {
			singleton = this;
			memset(_openvrIdToDeviceInfoMap, 0, sizeof(OpenvrWalkInPlaceInfo*) * vr::k_unMaxTrackedDeviceCount);
		}


		CServerDriver::~CServerDriver() {
			LOG(TRACE) << "CServerDriver::~CServerDriver_walkinplace()";
			// 10/10/2015 benj:  vrserver is exiting without calling Cleanup() to balance Init()
			// causing std::thread to call std::terminate

			// Apparently the above comment is not true anymore
			// Cleanup(); 
		}

		void CServerDriver::_poseUpatedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) {
			// Call rates:
			//
			// Vive HMD: 1120 calls/s
			// Vive Controller: 369 calls/s each
			//
			// Time is key. If we assume 1 HMD and 13 controllers, we have a total of  ~6000 calls/s. That's about 166 microseconds per call at 100% load.

			/*if (unWhichDevice == 0) {
			#define CALLCOUNTSAMPESIZE 1000
			static uint64_t callcount = 0;
			static uint64_t starttime = 0;
			callcount++;
			if (starttime == 0) {
			starttime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			} else if (callcount > CALLCOUNTSAMPESIZE) {
			auto endtime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			auto diff = endtime - starttime;
			LOG(INFO) << "HMD pose update: " << CALLCOUNTSAMPESIZE << " calls in " << diff << "ms: " << CALLCOUNTSAMPESIZE * 1000 / diff << " calls / s";
			starttime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			callcount = 0;
			}
			}*/
			if (true || (_openvrIdToDeviceInfoMap[unWhichDevice] && _openvrIdToDeviceInfoMap[unWhichDevice]->isValid())) {
				_openvrIdToDeviceInfoMap[unWhichDevice]->handleNewDevicePose(_this, _poseUpatedDetour.origFunc, unWhichDevice, newPose);
			}
			else {
				return _poseUpatedDetour.origFunc(_this, unWhichDevice, newPose, unPoseStructSize);
			}
		}

		void CServerDriver::_buttonPressedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			LOG(TRACE) << "Detour::buttonPressedDetourFunc(" << _this << ", " << unWhichDevice << ", " << (int)eButtonId << ", " << eventTimeOffset << ")";
			if (g_stepDetectEnabled && (eButtonId == vr::EVRButtonId::k_EButton_A || eButtonId == vr::EVRButtonId::k_EButton_Axis0) && !g_runPoseDetected) {
			} else {
				if (_openvrIdToDeviceInfoMap[unWhichDevice] && _openvrIdToDeviceInfoMap[unWhichDevice]->isValid()) {
					_openvrIdToDeviceInfoMap[unWhichDevice]->handleButtonEvent(_this, _buttonPressedDetour.origFunc, unWhichDevice, ButtonEventType::ButtonPressed, eButtonId, eventTimeOffset);
				}
				else {
					_buttonPressedDetour.origFunc(_this, unWhichDevice, eButtonId, eventTimeOffset);
				}
			}
			if (eButtonId == vr::EVRButtonId::k_EButton_Grip) {
				g_isHoldingAccuracyButton = true;
			}
		}

		void CServerDriver::_buttonUnpressedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			LOG(TRACE) << "Detour::buttonUnpressedDetourFunc(" << _this << ", " << unWhichDevice << ", " << (int)eButtonId << ", " << eventTimeOffset << ")";
			if (_openvrIdToDeviceInfoMap[unWhichDevice] && _openvrIdToDeviceInfoMap[unWhichDevice]->isValid()) {
				_openvrIdToDeviceInfoMap[unWhichDevice]->handleButtonEvent(_this, _buttonUnpressedDetour.origFunc, unWhichDevice, ButtonEventType::ButtonUnpressed, eButtonId, eventTimeOffset);
			}
			else {
				_buttonUnpressedDetour.origFunc(_this, unWhichDevice, eButtonId, eventTimeOffset);
			}
			if (eButtonId == vr::EVRButtonId::k_EButton_Grip) {
				g_isHoldingAccuracyButton = false;
			}
		}

		void CServerDriver::_buttonTouchedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			LOG(TRACE) << "Detour::buttonTouchedDetourFunc(" << _this << ", " << unWhichDevice << ", " << (int)eButtonId << ", " << eventTimeOffset << ")";
			if (_openvrIdToDeviceInfoMap[unWhichDevice] && _openvrIdToDeviceInfoMap[unWhichDevice]->isValid()) {
				_openvrIdToDeviceInfoMap[unWhichDevice]->handleButtonEvent(_this, _buttonTouchedDetour.origFunc, unWhichDevice, ButtonEventType::ButtonTouched, eButtonId, eventTimeOffset);
			}
			else {
				_buttonTouchedDetour.origFunc(_this, unWhichDevice, eButtonId, eventTimeOffset);
			}
		}

		void CServerDriver::_buttonUntouchedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, vr::EVRButtonId eButtonId, double eventTimeOffset) {
			LOG(TRACE) << "Detour::buttonUntouchedDetourFunc(" << _this << ", " << unWhichDevice << ", " << (int)eButtonId << ", " << eventTimeOffset << ")";
			if (_openvrIdToDeviceInfoMap[unWhichDevice] && _openvrIdToDeviceInfoMap[unWhichDevice]->isValid()) {
				_openvrIdToDeviceInfoMap[unWhichDevice]->handleButtonEvent(_this, _buttonUntouchedDetour.origFunc, unWhichDevice, ButtonEventType::ButtonUntouched, eButtonId, eventTimeOffset);
			}
			else {
				_buttonUntouchedDetour.origFunc(_this, unWhichDevice, eButtonId, eventTimeOffset);
			}
		}

		void CServerDriver::_axisUpdatedDetourFunc(vr::IVRServerDriverHost* _this, uint32_t unWhichDevice, uint32_t unWhichAxis, const vr::VRControllerAxis_t & axisState) {
			LOG(TRACE) << "Detour::axisUpdatedDetourFunc(" << _this << ", " << unWhichDevice << ", " << (int)unWhichAxis << ", <state>)";
			if (g_stepDetectEnabled && unWhichAxis == vr::EVRButtonId::k_EButton_Axis0 && !g_stepPoseDetected) {
			}
			else {
				if (_openvrIdToDeviceInfoMap[unWhichDevice] && _openvrIdToDeviceInfoMap[unWhichDevice]->isValid()) {
					_openvrIdToDeviceInfoMap[unWhichDevice]->handleAxisEvent(_this, _axisUpdatedDetour.origFunc, unWhichDevice, unWhichAxis, axisState);
				}
				else {
					_axisUpdatedDetour.origFunc(_this, unWhichDevice, unWhichAxis, axisState);
				}
			}
		}

		vr::EVRInitError CServerDriver::_deviceActivateDetourFunc(vr::ITrackedDeviceServerDriver* _this, uint32_t unObjectId) {
			LOG(TRACE) << "Detour::deviceActivateDetourFunc(" << _this << ", " << unObjectId << ")";
			auto i = _openvrDeviceInfos.find(_this);
			if (i != _openvrDeviceInfos.end()) {
				auto info = i->second;
				info->setOpenvrId(unObjectId);
				_openvrIdToDeviceInfoMap[unObjectId] = info.get();
				LOG(INFO) << "Detour::deviceActivateDetourFunc: sucessfully added to trackedDeviceInfos";
			}
			auto& d = _deviceActivateDetourMap.find(_this);
			if (d != _deviceActivateDetourMap.end()) {
				return d->second->origFunc(_this, unObjectId);
			}
			return vr::VRInitError_Unknown;
		}


		bool CServerDriver::_deviceAddedDetourFunc(vr::IVRServerDriverHost* _this, const char *pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, vr::ITrackedDeviceServerDriver *pDriver) {
			LOG(TRACE) << "Detour::deviceAddedDetourFunc(" << _this << ", " << pchDeviceSerialNumber << ", " << (int)eDeviceClass << ", " << pDriver << ")";

			// Redirect activate() function
			auto deviceActivatedOrig = (*((void***)pDriver))[0];
			_DetourFuncInfo<_DetourTrackedDeviceActivate_t>* foundEntry = nullptr;
			for (auto& d : _deviceActivateDetours) {
				if (d.targetFunc == deviceActivatedOrig) {
					foundEntry = &d;
					break;
				}
			}
			if (!foundEntry) {
				_deviceActivateDetours.emplace_back();
				auto& d = _deviceActivateDetours[_deviceActivateDetours.size() - 1];
				MH_STATUS mhError;
				CREATE_MH_HOOK(d, _deviceActivateDetourFunc, "deviceActivateDetour", pDriver, 0);
				foundEntry = &d;
			}
			_deviceActivateDetourMap.insert({ pDriver, foundEntry });

			// Create ManipulationInfo entry
			auto info = std::make_shared<OpenvrWalkInPlaceInfo>(pDriver, eDeviceClass, vr::k_unTrackedDeviceIndexInvalid, _this);
			_openvrDeviceInfos.emplace(pDriver, info);

			return _deviceAddedDetour.origFunc(_this, pchDeviceSerialNumber, eDeviceClass, pDriver);
		};


		vr::EVRInitError CServerDriver::Init(vr::IVRDriverContext *pDriverContext) {
			LOG(TRACE) << "CServerDriver::Init()";
			VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

			auto mhError = MH_Initialize();
			if (mhError == MH_OK) {
				CREATE_MH_HOOK(_deviceAddedDetour, _deviceAddedDetourFunc, "deviceAddedDetour", vr::VRServerDriverHost(), 0);
				CREATE_MH_HOOK(_poseUpatedDetour, _poseUpatedDetourFunc, "poseUpatedDetour", vr::VRServerDriverHost(), 1);
				CREATE_MH_HOOK(_buttonPressedDetour, _buttonPressedDetourFunc, "buttonPressedDetour", vr::VRServerDriverHost(), 3);
				CREATE_MH_HOOK(_buttonUnpressedDetour, _buttonUnpressedDetourFunc, "buttonUnpressedDetour", vr::VRServerDriverHost(), 4);
				CREATE_MH_HOOK(_buttonTouchedDetour, _buttonTouchedDetourFunc, "buttonTouchedDetour", vr::VRServerDriverHost(), 5);
				CREATE_MH_HOOK(_buttonUntouchedDetour, _buttonUntouchedDetourFunc, "buttonUntouchedDetour", vr::VRServerDriverHost(), 6);
				CREATE_MH_HOOK(_axisUpdatedDetour, _axisUpdatedDetourFunc, "axisUpdatedDetour", vr::VRServerDriverHost(), 7);
			}
			else {
				LOG(ERROR) << "Error while initialising minHook: " << MH_StatusToString(mhError);
			}

			// Start IPC thread
			shmCommunicator.init(this);
			return vr::VRInitError_None;
		}

		void CServerDriver::Cleanup() {
			LOG(TRACE) << "CServerDriver::Cleanup()";

			REMOVE_MH_HOOK(_deviceAddedDetour);
			REMOVE_MH_HOOK(_poseUpatedDetour);
			REMOVE_MH_HOOK(_buttonPressedDetour);
			REMOVE_MH_HOOK(_buttonUnpressedDetour);
			REMOVE_MH_HOOK(_buttonTouchedDetour);
			REMOVE_MH_HOOK(_buttonUntouchedDetour);
			REMOVE_MH_HOOK(_axisUpdatedDetour);

			MH_Uninitialize();
			shmCommunicator.shutdown();
			VR_CLEANUP_SERVER_DRIVER_CONTEXT();
		}

		// Call frequency: ~93Hz
		void CServerDriver::RunFrame() {
			/*#define CALLCOUNTSAMPESIZE 1000
			static uint64_t callcount = 0;
			static uint64_t starttime = 0;
			callcount++;
			if (starttime == 0) {
			starttime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			} else if (callcount > CALLCOUNTSAMPESIZE) {
			auto endtime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			auto diff = endtime - starttime;
			LOG(INFO) << "RunFrame: " << CALLCOUNTSAMPESIZE << " calls in " << diff << "ms: " << CALLCOUNTSAMPESIZE * 1000 / diff << " calls / s";
			starttime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			callcount = 0;
			}*/
		}

		OpenvrWalkInPlaceInfo* CServerDriver::walkInPlace_getInfo(uint32_t unWhichDevice) {
			std::lock_guard<std::recursive_mutex> lock(_openvrDevicesMutex);
			if (_openvrIdToDeviceInfoMap[unWhichDevice] && _openvrIdToDeviceInfoMap[unWhichDevice]->isValid()) {
				return _openvrIdToDeviceInfoMap[unWhichDevice];
			}
			return nullptr;
		}

		void CServerDriver::enableStepDetection(bool enable) {
			_stepPoseDetectEnabled = enable;
			g_stepDetectEnabled = enable;
			for (auto d : _openvrDeviceInfos) {
				if (enable) {
					d.second->setStepDetectionMode();
				}
				else {
					d.second->setDefaultMode();
				}
			}
		}

		void CServerDriver::setGameStepType(int gameType) {
			_gameStepType = gameType;
			if (_gameStepType == 3) {
				g_runPoseDetected = true;
			}
		}

		void CServerDriver::setStepIntSec(float value) {
			_stepIntegrateStepLimit = (double)value;
		}

		void CServerDriver::setWalkTouch(float value) {
			walkTouch = value;
		}

		void CServerDriver::setJogTouch(float value) {
			jogTouch = value;
		}

		void CServerDriver::setRunTouch(float value) {
			runTouch = value;
		}

		void CServerDriver::setHMDThreshold(vr::HmdVector3d_t value) {
			_hmdThreshold = value;
		}

		void CServerDriver::setAccuracyButton(bool enable) {
			_useAccuracyButton = enable;
		}

		void CServerDriver::setHandJogThreshold(float value) {
			_handJogThreshold = value;
		}

		void CServerDriver::setHandRunThreshold(float value) {
			_handJogThreshold = value;
		}

		void CServerDriver::setStepPoseDetected(bool enable) {
			_stepPoseDetected = enable;
		}

		bool CServerDriver::isStepDetectionEnabled() {
			return this->_stepPoseDetectEnabled;
		}

		bool CServerDriver::accuracyButtonEnabled() {
			return this->_useAccuracyButton;
		}

		bool CServerDriver::_applyStepPoseDetect(vr::DriverPose_t& pose, OpenvrWalkInPlaceInfo* deviceInfo) {
			if (this->_stepPoseDetectEnabled && pose.poseIsValid) {
				double deltatime = 1.0 / 180.0;
				auto now = std::chrono::duration_cast <std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				double tdiff = (double)(now - _timeLastTick);
				if (true || tdiff >= deltatime) {
					vr::ETrackedDeviceClass deviceClass = deviceInfo->deviceClass();
					bool isHMD = deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD;
					if (!this->_stepPoseDetected) {
						if (isHMD) {
							/*vr::HmdQuaternion_t tmpConj = vrmath::quaternionConjugate(pose.qWorldFromDriverRotation);
							auto poseWorldRot = tmpConj * pose.qRotation;

							double zsqr = poseWorldRot.z * poseWorldRot.z;

							// roll (x-axis rotation)
							double t0 = +2.0 * (poseWorldRot.w * poseWorldRot.x + poseWorldRot.y * poseWorldRot.z);
							double t1 = +1.0 - 2.0 * (zsqr + poseWorldRot.x * poseWorldRot.x);
							double roll = (180*std::atan2(t0, t1))/M_PI;

							// pitch (z-axis rotation)
							double t2 = +2.0 * (poseWorldRot.w * poseWorldRot.z + poseWorldRot.y * poseWorldRot.x);
							t2 = t2 > 1.0 ? 1.0 : t2;
							t2 = t2 < -1.0 ? -1.0 : t2;
							double pitch = (180*std::asin(t2))/M_PI;

							LOG(INFO) << "HMD Rot: " << roll << "," << pitch;*/
							//LOG(INFO) << "HMD Step: " << pose.vecAcceleration[0] << "," << pose.vecAcceleration[1] << "," << pose.vecAcceleration[2];
							//LOG(INFO) << "HMD Rot: " << pose.qRotation.x << "," << pose.qRotation.y << "," << pose.qRotation.z;
							//LOG(INFO) << "HMD Step: " << pose.vecVelocity[0] << "," << pose.vecVelocity[1] << "," << pose.vecVelocity[2];
							//LOG(INFO) << "HMD POS: " << pose.vecPosition[0] << " " << pose.vecPosition[1] << " " << pose.vecPosition[2];
							if (isTakingStep(pose.vecVelocity, _hmdThreshold, 0, 0)) { //&& isStepingInPlace(pose.vecPosition) ) {
								_openvrDeviceStepPoseTracker[0] = -1;
								_openvrDeviceStepPoseTracker[1] = -1;
								_openvrDeviceStepPoseTracker[2] = -1;
							}
							else {
								_openvrDeviceStepPoseTracker[0] = 0;
								_openvrDeviceStepPoseTracker[1] = 0;
								_openvrDeviceStepPoseTracker[2] = 0;
							}
						}
						//else if ( deviceInfo->deviceClass() == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
							//LOG(INFO) << "CONTROLLER MAG VEL: " << pose.vecVelocity[0] + pose.vecVelocity[1] + pose.vecVelocity[2];
						//}
						if (_openvrDeviceStepPoseTracker[0] != 0) {
							//&& _openvrDeviceStepPoseTracker[1] != 0 && _openvrDeviceStepPoseTracker[2] != 0 ) {
							//&& (_openvrDeviceStepPoseTracker[1] != _openvrDeviceStepPoseTracker[2])) {
							this->setStepPoseDetected(true);
							g_stepPoseDetected = true;
							_openvrDeviceStepPoseTracker[1] = -1;
							_openvrDeviceStepPoseTracker[2] = -1;
						}
					}
					if (this->_stepPoseDetected) {
						if (_openvrDeviceStepPoseTracker[0] == 0) {
							if ((deviceInfo->openvrId() == _openvrDeviceStepPoseTracker[1]
								|| deviceInfo->openvrId() == _openvrDeviceStepPoseTracker[2])) {
								vr::IVRServerDriverHost* driverHost;
								int deviceId = deviceInfo->openvrId();
								if (_openvrIdToDeviceInfoMap[deviceId] && _openvrIdToDeviceInfoMap[deviceId]->isValid()) {
									driverHost = _openvrIdToDeviceInfoMap[deviceId]->driverHost();
								}
								else {
									driverHost = vr::VRServerDriverHost();
								}
								if (_gameStepType != 3) {
									vr::VRControllerAxis_t axisState;
									axisState.x = 0;
									axisState.y = walkTouch;
									bool isRunning = isRunningStep(pose.vecVelocity);
									g_runPoseDetected = isRunning;
									if (isRunning) {
										axisState.y = runTouch;
									}
									else if (isJoggingStep(pose.vecVelocity)) {
										axisState.y = jogTouch;
										g_jogPoseDetected = true;
									}
									driverHost->TrackedDeviceButtonTouched(deviceId, vr::k_EButton_Axis0, 0);
									driverHost->TrackedDeviceAxisUpdated(deviceId, 0, axisState);
									if (_gameStepType != 2) {
										if (isRunning) {
											driverHost->TrackedDeviceButtonPressed(deviceId, vr::EVRButtonId::k_EButton_Axis0, 0);
										}
									}
									this->_hasUnTouchedStepAxis = 0;
								}
								else {
									if (_teleportUnpressed) {
										driverHost->TrackedDeviceButtonPressed(deviceId, vr::EVRButtonId::k_EButton_Axis0, 0);
										_teleportUnpressed = false;
									}
									else {
										driverHost->TrackedDeviceButtonUnpressed(deviceId, vr::EVRButtonId::k_EButton_Axis0, 0);
										_teleportUnpressed = true;
									}
								}
							}
						}
						if (isHMD) {
							//if ( isStepingInPlace(pose.vecPosition) ) 
							//{

							/*vr::HmdQuaternion_t tmpConj = vrmath::quaternionConjugate(pose.qWorldFromDriverRotation);
							auto poseWorldRot = tmpConj * pose.qRotation;

							double zsqr = poseWorldRot.z * poseWorldRot.z;

							// roll (x-axis rotation)
							double t0 = +2.0 * (poseWorldRot.w * poseWorldRot.x + poseWorldRot.y * poseWorldRot.z);
							double t1 = +1.0 - 2.0 * (zsqr + poseWorldRot.x * poseWorldRot.x);
							double roll = (180*std::atan2(t0, t1))/M_PI;

							// pitch (z-axis rotation)
							double t2 = +2.0 * (poseWorldRot.w * poseWorldRot.z + poseWorldRot.y * poseWorldRot.x);
							t2 = t2 > 1.0 ? 1.0 : t2;
							t2 = t2 < -1.0 ? -1.0 : t2;
							double pitch = (180*std::asin(t2))/M_PI;

							LOG(INFO) << "HMD Rot: " << roll << "," << pitch;*/

							if (isTakingStep(pose.vecVelocity, _hmdThreshold, 0, 0)) {
								_stepIntegrateSteps = 0;
							}
							if (_openvrDeviceStepPoseTracker[0] == 0) {
								//_stepIntegrateSteps += tdiff;
							}
							//}
							/*else {
							//DEVIATED FROM STEP SPACE - STOP MOVEMENT
							_stepIntegrateSteps = 999;
							_openvrDeviceStepPoseTracker[0] = 999;
							}*/
						}
						else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
							if (_openvrDeviceStepPoseTracker[0] != 0) {
								if (_openvrDeviceStepPoseTracker[1] < 0) {
									_openvrDeviceStepPoseTracker[1] = deviceInfo->openvrId();
								}
								else if (_openvrDeviceStepPoseTracker[2] < 0) {
									_openvrDeviceStepPoseTracker[2] = deviceInfo->openvrId();
									_openvrDeviceStepPoseTracker[0] = 0;
								}
							}
							else if (isJoggingStep(pose.vecVelocity) || isRunningStep(pose.vecVelocity)) {
								_stepIntegrateSteps = 0;
							}
						}
						_stepIntegrateSteps += tdiff;
						if (_stepIntegrateSteps >= 0.07) { //_stepIntegrateStepLimit) {
							this->setStepPoseDetected(false);
							_stepIntegrateSteps = 0.0;
							_handsPointDir = { 0.0, 0.0, 0.0 };
							_timeLastStepTaken = now;
							_openvrDeviceStepPoseTracker[0] = 0;
							_openvrDeviceStepPoseTracker[1] = 0;
							_openvrDeviceStepPoseTracker[2] = 0;
							g_stepPoseDetected = false;
							g_jogPoseDetected = false;
							if (_gameStepType != 3) {
								g_runPoseDetected = false;
							}
						}
					}
					else {
						if (this->_hasUnTouchedStepAxis < 50) {
							vr::IVRServerDriverHost* driverHost;
							int deviceId = deviceInfo->openvrId();
							if (_openvrIdToDeviceInfoMap[deviceId] && _openvrIdToDeviceInfoMap[deviceId]->isValid()) {
								driverHost = _openvrIdToDeviceInfoMap[deviceId]->driverHost();
							}
							else {
								driverHost = vr::VRServerDriverHost();
							}
							if (_gameStepType != 3) {
								vr::VRControllerAxis_t axisState;
								axisState.x = 0;
								axisState.y = 0;
								if (_gameStepType != 2) {
									driverHost->TrackedDeviceButtonUnpressed(deviceId, vr::EVRButtonId::k_EButton_Axis0, 0);
								}
								driverHost->TrackedDeviceAxisUpdated(deviceId, 0, axisState);
								driverHost->TrackedDeviceButtonUntouched(deviceId, vr::k_EButton_Axis0, 0);
							}
							else {
								driverHost->TrackedDeviceButtonUnpressed(deviceId, vr::EVRButtonId::k_EButton_Axis0, 0);
							}
							this->_hasUnTouchedStepAxis++;
							_teleportUnpressed = true;
						}
					}
					_timeLastTick = now;
				}
			}

			return false;
		}

		bool CServerDriver::isTakingStep(double * vel, vr::HmdVector3d_t threshold, double roll, double pitch) {
			bool stepParams = ((std::abs(vel[2]) < threshold.v[2]) &&
				(std::abs(vel[0]) < threshold.v[0]) &&
				((vel[1] > threshold.v[1] && (vel[1] > vel[0] && vel[1] > vel[2]))
					|| (vel[1] < -1 * threshold.v[1] && (vel[1] < vel[0] && vel[1] < vel[2]))));
			//stepParams = stepParams && (roll < 20 && pitch < 20);
			stepParams = stepParams && (!_useAccuracyButton || g_isHoldingAccuracyButton);
			return stepParams;
		}

		bool CServerDriver::isShakingHead(double * vel, vr::HmdVector3d_t threshold) {
			bool stepParams = ((std::abs(vel[2]) > threshold.v[2]) ||
				(std::abs(vel[0]) > threshold.v[0]));
			return stepParams;
		}

		bool CServerDriver::isStepingInPlace(double * pos) {
			vr::HmdVector3d_t currentDeviation = { 0.0, 0.0, 0.0 };
			currentDeviation.v[0] = std::abs(pos[0] - _avgStepPos.v[0]);
			currentDeviation.v[1] = std::abs(pos[1] - _avgStepPos.v[1]);
			currentDeviation.v[2] = std::abs(pos[2] - _avgStepPos.v[2]);
			_avgStepPos.v[0] = (_avgStepPos.v[0] + pos[0]) / 2.0;
			_avgStepPos.v[1] = (_avgStepPos.v[1] + pos[1]) / 2.0;
			_avgStepPos.v[2] = (_avgStepPos.v[2] + pos[2]) / 2.0;
			return (
				(
					pos[0] < (_avgStepPos.v[0] + _stdDeviation.v[0])
					&& pos[0] > (_avgStepPos.v[0] - _stdDeviation.v[0])
					)
				&&
				(
					pos[1] < (_avgStepPos.v[1] + _stdDeviation.v[1])
					&& pos[1] > (_avgStepPos.v[1] - _stdDeviation.v[1])
					)
				&&
				(
					pos[2] < (_avgStepPos.v[2] + _stdDeviation.v[2])
					&& pos[2] > (_avgStepPos.v[2] - _stdDeviation.v[2])
					));
		}

		bool CServerDriver::isJoggingStep(double * vel) {
			float magVel = (std::abs(vel[0]) + std::abs(vel[1]) + std::abs(vel[2]));
			return (magVel > _handJogThreshold);
		}


		bool CServerDriver::isRunningStep(double * vel) {
			float magVel = (std::abs(vel[0]) + std::abs(vel[1]) + std::abs(vel[2]));
			return (magVel > _handRunThreshold);
		}

	} // end namespace driver
} // end namespace vrwalkinplace
