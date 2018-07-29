#include "WalkInPlaceTabController.h"
#include <QQuickWindow>
#include <QApplication>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QDebug>
#include <QtCore/QtMath>
#include "../overlaycontroller.h"
#include <openvr_math.h>
#include <chrono>
// application namespace
namespace walkinplace {

	WalkInPlaceTabController::~WalkInPlaceTabController() {
		if (identifyThread.joinable()) {
			identifyThread.join();
		}
	}


	void WalkInPlaceTabController::initStage1() {

		reloadWalkInPlaceProfiles();
		reloadWalkInPlaceSettings();
	}


	void WalkInPlaceTabController::initStage2(OverlayController * parent, QQuickWindow * widget) {
		this->parent = parent;
		this->widget = widget;
		try {
			for (uint32_t id = 0; id < vr::k_unMaxTrackedDeviceCount; ++id) {
				auto deviceClass = vr::VRSystem()->GetTrackedDeviceClass(id);
				if (deviceClass != vr::TrackedDeviceClass_Invalid) {
					if (deviceClass == vr::TrackedDeviceClass_HMD || deviceClass == vr::TrackedDeviceClass_Controller || deviceClass == vr::TrackedDeviceClass_GenericTracker) {
						auto info = std::make_shared<DeviceInfo>();
						info->openvrId = id;
						info->deviceClass = deviceClass;
						char buffer[vr::k_unMaxPropertyStringSize];
						vr::ETrackedPropertyError pError = vr::TrackedProp_Success;
						vr::VRSystem()->GetStringTrackedDeviceProperty(id, vr::Prop_SerialNumber_String, buffer, vr::k_unMaxPropertyStringSize, &pError);
						if (pError == vr::TrackedProp_Success) {
							info->serial = std::string(buffer);
						}
						else {
							info->serial = std::string("<unknown serial>");
							LOG(ERROR) << "Could not get serial of device " << id;
						}
						info->deviceMode = 0;
						deviceInfos.push_back(info);
						if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
							if (_controllerDeviceIds[0] < 0) {
								_controllerDeviceIds[0] = info->openvrId;
							}
							else if (_controllerDeviceIds[1] < 0) {
								_controllerDeviceIds[1] = info->openvrId;
							}
						}
						LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
					}
					maxValidDeviceId = id;
				}
			}
			emit deviceCountChanged((unsigned)deviceInfos.size());
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Could not connect to driver component: " << e.what();
		}
	}


	void WalkInPlaceTabController::eventLoopTick() {
		if (stepDetectEnabled) {
			applyStepPoseDetect();
		}
		if (identifyControlTimerSet) {
			auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			double tdiff = ((double)(now - identifyControlLastTime));
			//LOG(INFO) << "DT: " << tdiff;
			if (tdiff >= identifyControlTimeOut) {
				identifyControlTimerSet = false;
				setDeviceRenderModel(controlSelectOverlayHandle, 0, 1, 1, 1, 1, 1, 1);
			}
		}
		if (settingsUpdateCounter >= 50) {
			settingsUpdateCounter = 0;
			if (showingStepGraph < 100) {
				showingStepGraph++;
				if (showingStepGraph >= 100) {
				}
			}
			if (parent->isDashboardVisible() || parent->isDesktopMode()) {
				bool newDeviceAdded = false;
				for (uint32_t id = maxValidDeviceId + 1; id < vr::k_unMaxTrackedDeviceCount; ++id) {
					auto deviceClass = vr::VRSystem()->GetTrackedDeviceClass(id);
					if (deviceClass != vr::TrackedDeviceClass_Invalid) {
						if (deviceClass == vr::TrackedDeviceClass_HMD || deviceClass == vr::TrackedDeviceClass_Controller || deviceClass == vr::TrackedDeviceClass_GenericTracker) {
							auto info = std::make_shared<DeviceInfo>();
							info->openvrId = id;
							info->deviceClass = deviceClass;
							char buffer[vr::k_unMaxPropertyStringSize];
							vr::ETrackedPropertyError pError = vr::TrackedProp_Success;
							vr::VRSystem()->GetStringTrackedDeviceProperty(id, vr::Prop_SerialNumber_String, buffer, vr::k_unMaxPropertyStringSize, &pError);
							if (pError == vr::TrackedProp_Success) {
								info->serial = std::string(buffer);
							}
							else {
								info->serial = std::string("<unknown serial>");
								LOG(ERROR) << "Could not get serial of device " << id;
							}

							info->deviceMode = 0;
							deviceInfos.push_back(info);
							if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
								if (_controllerDeviceIds[0] < 0) {
									_controllerDeviceIds[0] = info->openvrId;
								}
								else if (_controllerDeviceIds[1] < 0) {
									_controllerDeviceIds[1] = info->openvrId;
								}
							}
							LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
							newDeviceAdded = true;
						}
						maxValidDeviceId = id;
					}
				}
				if (newDeviceAdded) {
					emit deviceCountChanged((unsigned)deviceInfos.size());
				}
			}
		}
		else {
			settingsUpdateCounter++;
		}
	}

	void WalkInPlaceTabController::handleEvent(const vr::VREvent_t&) {
		/*switch (vrEvent.eventType) {
		default:
		break;
		}*/
	}

	unsigned  WalkInPlaceTabController::getDeviceCount() {
		return (unsigned)deviceInfos.size();
	}

	QString WalkInPlaceTabController::getDeviceSerial(unsigned index) {
		if (index < deviceInfos.size()) {
			return QString::fromStdString(deviceInfos[index]->serial);
		}
		else {
			return QString("<ERROR>");
		}
	}

	unsigned WalkInPlaceTabController::getDeviceId(unsigned index) {
		if (index < deviceInfos.size()) {
			return (int)deviceInfos[index]->openvrId;
		}
		else {
			return vr::k_unTrackedDeviceIndexInvalid;
		}
	}

	int WalkInPlaceTabController::getDeviceClass(unsigned index) {
		if (index < deviceInfos.size()) {
			return (int)deviceInfos[index]->deviceClass;
		}
		else {
			return -1;
		}
	}

	int WalkInPlaceTabController::getDeviceState(unsigned index) {
		if (index < deviceInfos.size()) {
			return (int)deviceInfos[index]->deviceStatus;
		}
		else {
			return -1;
		}
	}

	int WalkInPlaceTabController::getDeviceMode(unsigned index) {
		if (index < deviceInfos.size()) {
			return (int)deviceInfos[index]->deviceMode;
		}
		else {
			return -1;
		}
	}

	bool WalkInPlaceTabController::isStepDetectionEnabled() {
		return stepDetectEnabled;
	}

	double WalkInPlaceTabController::getStepTime() {
		return (_stepIntegrateStepLimit / 1000);
	}

	int WalkInPlaceTabController::getGameType() {
		return gameType;
	}

	int WalkInPlaceTabController::getHMDType() {
		return hmdType;
	}

	int WalkInPlaceTabController::getControlSelect() {
		return controlSelect;
	}

	int WalkInPlaceTabController::getAccuracyButtonControlSelect() {
		return buttonControlSelect;
	}

	int WalkInPlaceTabController::getAccuracyButton() {
		return useAccuracyButton;
	}

	bool WalkInPlaceTabController::getAccuracyButtonIsToggle() {
		return useButtonAsToggle;
	}

	bool WalkInPlaceTabController::getAccuracyButtonFlip() {
		return flipButtonUse;
	}

	int WalkInPlaceTabController::getHMDPitchDown() {
		return hmdPitchDown;
	}

	int WalkInPlaceTabController::getHMDPitchUp() {
		return hmdPitchUp;
	}

	float WalkInPlaceTabController::getHMDXZThreshold() {
		return _hmdThreshold.v[0];
	}

	float WalkInPlaceTabController::getHMDYThreshold() {
		return _hmdThreshold.v[1];
	}

	float WalkInPlaceTabController::getTrackerXZThreshold() {
		return _trackerThreshold.v[0];
	}

	float WalkInPlaceTabController::getTrackerYThreshold() {
		return _trackerThreshold.v[1];
	}

	bool WalkInPlaceTabController::getUseTrackers() {
		return useTrackers;
	}

	bool WalkInPlaceTabController::getDisableHMD() {
		return disableHMD;
	}

	float WalkInPlaceTabController::getHandWalkThreshold() {
		return handWalkThreshold;
	}

	float WalkInPlaceTabController::getHandJogThreshold() {
		return handJogThreshold;
	}

	float WalkInPlaceTabController::getHandRunThreshold() {
		return handRunThreshold;
	}

	bool WalkInPlaceTabController::getScaleTouchWithSwing() {
		return scaleSpeedWithSwing;
	}

	float WalkInPlaceTabController::getWalkTouch() {
		return walkTouch;
	}

	float WalkInPlaceTabController::getJogTouch() {
		return jogTouch;
	}

	float WalkInPlaceTabController::getRunTouch() {
		return runTouch;
	}

	void WalkInPlaceTabController::setupStepGraph() {
		//auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		//_timeLastGraphPoint = now;
		showingStepGraph = 0;
	}

	QList<qreal> WalkInPlaceTabController::getGraphPoses() {
		showingStepGraph = 0;
		if (!stepDetectEnabled) {
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
		}
		bool firstController = true;
		bool firstTracker = true;
		for (auto info : deviceInfos) {
			if (latestDevicePoses[info->openvrId].bPoseIsValid) {
				if (info->deviceClass == vr::TrackedDeviceClass_HMD) {
					if (hmdType != 1) {
						if (!stepDetectEnabled) {
							auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							double tdiff = ((double)(now - _velStepTime)) / 1000.0;
							auto m = latestDevicePoses[info->openvrId].mDeviceToAbsoluteTracking.m;

							hmdVel.v[0] = (m[0][3] - lastHmdPos.v[0]) / tdiff;
							hmdVel.v[1] = (m[1][3] - lastHmdPos.v[1]) / tdiff;
							hmdVel.v[2] = (m[2][3] - lastHmdPos.v[2]) / tdiff;

							lastHmdPos.v[0] = m[0][3];
							lastHmdPos.v[1] = m[1][3];
							lastHmdPos.v[2] = m[2][3];

							_velStepTime = now;
						}
					}
					else {
						hmdVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
						hmdVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
						hmdVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];
					}

					/*vr::HmdQuaternion_t tmpConj = vrmath::quaternionConjugate(pose.qWorldFromDriverRotation);
					auto poseWorldRot = tmpConj * pose.qRotation;

					double zsqr = poseWorldRot.z * poseWorldRot.z;

					// roll (x-axis rotation)
					double t0 = +2.0 * (poseWorldRot.w * poseWorldRot.x + poseWorldRot.z * poseWorldRot.y);
					double t1 = +1.0 - 2.0 * (poseWorldRot.x * poseWorldRot.x + zsqr);
					double roll = (180 * std::atan2(t0, t1)) / M_PI;

					// pitch (z-axis rotation)
					double t2 = +2.0 * (poseWorldRot.w * poseWorldRot.z - poseWorldRot.y * poseWorldRot.x);
					double pitch;
					if (std::fabs(t2) >= 1) {
					pitch = 90;
					}
					else {
					pitch = (180 * std::asin(t2)) / M_PI;
					}

					double t3 = +2.0 * (poseWorldRot.w * poseWorldRot.y + poseWorldRot.x * poseWorldRot.z);
					double t4 = +1.0 - 2.0 * (zsqr + poseWorldRot.y * poseWorldRot.y);
					double yaw = (180 * std::atan2(t3, t4)) / M_PI;*/
				}
				else if (info->deviceClass == vr::TrackedDeviceClass_Controller) {
					if (firstController) {
						cont1Vel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
						cont1Vel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
						cont1Vel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];
						firstController = false;
					}
					else {
						cont2Vel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
						cont2Vel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
						cont2Vel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];
					}
				}
				else if (info->deviceClass == vr::TrackedDeviceClass_GenericTracker) {
					if (firstTracker) {
						tracker1Vel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
						tracker1Vel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
						tracker1Vel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];
						firstTracker = false;
					}
					else {
						tracker2Vel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
						tracker2Vel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
						tracker2Vel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];
					}
				}
			}
		}
		QList<qreal> vals;
		vals.push_back(hmdVel.v[0]);
		vals.push_back(hmdVel.v[1]);
		vals.push_back(hmdVel.v[2]);

		vals.push_back(cont1Vel.v[0]);
		vals.push_back(cont1Vel.v[1]);
		vals.push_back(cont1Vel.v[2]);

		vals.push_back(cont2Vel.v[0]);
		vals.push_back(cont2Vel.v[1]);
		vals.push_back(cont2Vel.v[2]);

		vals.push_back(tracker1Vel.v[0]);
		vals.push_back(tracker1Vel.v[1]);
		vals.push_back(tracker1Vel.v[2]);

		vals.push_back(tracker2Vel.v[0]);
		vals.push_back(tracker2Vel.v[1]);
		vals.push_back(tracker2Vel.v[2]);
		//LOG(INFO) << "HMD VALS: " << hmdVel.v[0] << "," << hmdVel.v[1] << "," << hmdVel.v[2];
		/*if (g_AccuracyButton >= 0 && _controllerDeviceIds[0] >= 0 && _controllerDeviceIds[1] >= 0) {
		g_isHoldingAccuracyButton = false;
		g_isHoldingAccuracyButton1 = false;
		g_isHoldingAccuracyButton2 = false;
		updateAccuracyButtonState(_controllerDeviceIds[0], true);
		updateAccuracyButtonState(_controllerDeviceIds[1], false);
		}*/
		if (g_runPoseDetected) {
			vals.push_back(3);
		}
		else if (g_jogPoseDetected) {
			vals.push_back(2);
		}
		else if (_stepPoseDetected) {
			vals.push_back(1);
		}
		else {
			vals.push_back(0);
		}

		return vals;

	}

	bool WalkInPlaceTabController::isStepDetected() {
		return _stepPoseDetected;
	}

	void WalkInPlaceTabController::reloadWalkInPlaceSettings() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		settings->endGroup();
	}

	void WalkInPlaceTabController::reloadWalkInPlaceProfiles() {
		walkInPlaceProfiles.clear();
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		auto profileCount = settings->beginReadArray("walkInPlaceProfiles");
		for (int i = 0; i < profileCount; i++) {
			settings->setArrayIndex(i);
			walkInPlaceProfiles.emplace_back();
			auto& entry = walkInPlaceProfiles[i];
			entry.profileName = settings->value("profileName").toString().toStdString();
			entry.stepDetectionEnabled = settings->value("stepDetectionEnabled", false).toBool();
			entry.gameType = settings->value("gameType", 0).toInt();
			entry.hmdType = settings->value("hmdType", 0).toInt();
			entry.controlSelect = settings->value("controlSelect", 2).toInt();
			entry.buttonControlSelect = settings->value("buttonControlSelect", 2).toInt();
			entry.hmdThreshold_y = settings->value("hmdthreshold_y", 0.27).toFloat();
			entry.hmdThreshold_xz = settings->value("hmdthreshold_xz", 0.27).toFloat();
			entry.trackerThreshold_y = settings->value("trackerthreshold_y", 0.27).toFloat();
			entry.trackerThreshold_xz = settings->value("trackerthreshold_xz", 0.27).toFloat();
			entry.useTrackers = settings->value("useTrackers", false).toBool();
			entry.disableHMD = settings->value("disableHMD", false).toBool();
			entry.handJogThreshold = settings->value("handJog", 0.35).toFloat();
			entry.handRunThreshold = settings->value("handRun", 1.7).toFloat();
			entry.scaleTouchWithSwing = settings->value("scaleTouchWithSwing", false).toBool();
			entry.stepTime = settings->value("stepTime", 0.5).toDouble();
			entry.useAccuracyButton = settings->value("useAccuracyButton", 0).toInt();
			//entry.hmdPitchDown = settings->value("hmdPitchDown", 20).toInt();
			//entry.hmdPitchUp = settings->value("hmdPitchUp", 15).toInt();
			entry.walkTouch = settings->value("walkTouch", 0.6).toFloat();
			entry.jogTouch = settings->value("jogTouch", 0.87).toFloat();
			entry.runTouch = settings->value("runTouch", 1).toFloat();
			entry.useButtonAsToggle = settings->value("useButtonAsToggle", false).toBool();
			entry.flipButtonUse = settings->value("flipButtonUse", false).toBool();
		}
		settings->endArray();
		settings->endGroup();
	}

	void WalkInPlaceTabController::saveWalkInPlaceSettings() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		settings->endGroup();
		settings->sync();
	}


	void WalkInPlaceTabController::saveWalkInPlaceProfiles() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		settings->beginWriteArray("walkInPlaceProfiles");
		unsigned i = 0;
		for (auto& p : walkInPlaceProfiles) {
			settings->setArrayIndex(i);
			settings->setValue("profileName", QString::fromStdString(p.profileName));
			settings->setValue("stepDetectionEnabled", p.stepDetectionEnabled);
			settings->setValue("gameType", p.gameType);
			settings->setValue("hmdType", p.hmdType);
			settings->setValue("controlSelect", p.controlSelect);
			settings->setValue("buttonControlSelect", p.buttonControlSelect);
			settings->setValue("hmdthreshold_y", p.hmdThreshold_y);
			settings->setValue("hmdthreshold_xz", p.hmdThreshold_xz);
			settings->setValue("trackerthreshold_y", p.trackerThreshold_y);
			settings->setValue("trackerthreshold_xz", p.trackerThreshold_xz);
			settings->setValue("useTrackers", p.useTrackers);
			settings->setValue("disableHMD", p.disableHMD);
			settings->setValue("handJog", p.handJogThreshold);
			settings->setValue("handRun", p.handRunThreshold);
			settings->setValue("stepTime", p.stepTime);
			settings->setValue("useAccuracyButton", p.useAccuracyButton);
			//settings->setValue("hmdPitchDown", p.hmdPitchDown);
			//settings->setValue("hmdPitchUp", p.hmdPitchUp);
			settings->setValue("walkTouch", p.walkTouch);
			settings->setValue("jogTouch", p.jogTouch);
			settings->setValue("runTouch", p.runTouch);
			settings->setValue("useButtonAsToggle", p.useButtonAsToggle);
			settings->setValue("flipButtonUse", p.flipButtonUse);
			i++;
		}
		settings->endArray();
		settings->endGroup();
		settings->sync();
	}

	unsigned WalkInPlaceTabController::getWalkInPlaceProfileCount() {
		return (unsigned)walkInPlaceProfiles.size();
	}

	QString WalkInPlaceTabController::getWalkInPlaceProfileName(unsigned index) {
		if (index >= walkInPlaceProfiles.size()) {
			return QString();
		}
		else {
			return QString::fromStdString(walkInPlaceProfiles[index].profileName);
		}
	}

	void WalkInPlaceTabController::addWalkInPlaceProfile(QString name) {
		WalkInPlaceProfile* profile = nullptr;
		for (auto& p : walkInPlaceProfiles) {
			if (p.profileName.compare(name.toStdString()) == 0) {
				profile = &p;
				break;
			}
		}
		if (!profile) {
			auto i = walkInPlaceProfiles.size();
			walkInPlaceProfiles.emplace_back();
			profile = &walkInPlaceProfiles[i];
		}
		profile->profileName = name.toStdString();
		profile->stepDetectionEnabled = isStepDetectionEnabled();
		profile->gameType = gameType;
		profile->hmdType = hmdType;
		profile->controlSelect = controlSelect;
		profile->buttonControlSelect = buttonControlSelect;
		profile->hmdThreshold_y = _hmdThreshold.v[1];
		profile->hmdThreshold_xz = _hmdThreshold.v[0];
		profile->trackerThreshold_y = (float)_trackerThreshold.v[1];
		profile->trackerThreshold_xz = (float)_trackerThreshold.v[0];
		profile->useTrackers = useTrackers || disableHMD;
		profile->disableHMD = disableHMD;
		profile->handJogThreshold = handJogThreshold;
		profile->handRunThreshold = handRunThreshold;
		profile->scaleTouchWithSwing = scaleSpeedWithSwing;
		profile->stepTime = (_stepIntegrateStepLimit / 1000.0);
		profile->useAccuracyButton = useAccuracyButton;
		//profile->hmdPitchDown = hmdPitchDown;
		//profile->hmdPitchUp = hmdPitchUp;
		profile->walkTouch = walkTouch;
		profile->jogTouch = jogTouch;
		profile->runTouch = runTouch;
		profile->useButtonAsToggle = useButtonAsToggle;
		profile->flipButtonUse = flipButtonUse;
		saveWalkInPlaceProfiles();
		OverlayController::appSettings()->sync();
		emit walkInPlaceProfilesChanged();
	}

	void WalkInPlaceTabController::applyWalkInPlaceProfile(unsigned index) {
		if (index < walkInPlaceProfiles.size()) {
			auto& profile = walkInPlaceProfiles[index];
			gameType = profile.gameType;
			hmdType = profile.hmdType;
			controlSelect = profile.controlSelect;
			buttonControlSelect = profile.buttonControlSelect;
			_hmdThreshold.v[0] = profile.hmdThreshold_xz;
			_hmdThreshold.v[1] = profile.hmdThreshold_y;
			_hmdThreshold.v[2] = profile.hmdThreshold_xz;
			_trackerThreshold.v[0] = profile.trackerThreshold_xz;
			_trackerThreshold.v[1] = profile.trackerThreshold_y;
			_trackerThreshold.v[2] = profile.trackerThreshold_xz;
			useTrackers = profile.useTrackers || profile.disableHMD;
			disableHMD = profile.disableHMD;
			handJogThreshold = profile.handJogThreshold;
			handRunThreshold = profile.handRunThreshold;
			scaleSpeedWithSwing = profile.scaleTouchWithSwing;
			_stepIntegrateStepLimit = profile.stepTime * 1000;
			useAccuracyButton = profile.useAccuracyButton;
			//hmdPitchDown = profile.hmdPitchDown;
			//hmdPitchUp = profile.hmdPitchUp;
			walkTouch = profile.walkTouch;
			jogTouch = profile.jogTouch;
			runTouch = profile.runTouch;
			useButtonAsToggle = profile.useButtonAsToggle;
			flipButtonUse = profile.flipButtonUse;

			enableStepDetection(profile.stepDetectionEnabled);
			setGameStepType(profile.gameType);
			setHMDType(profile.hmdType);
			setControlSelect(profile.controlSelect);
			setAccuracyButtonControlSelect(profile.buttonControlSelect);
			setHMDThreshold(profile.hmdThreshold_xz, profile.hmdThreshold_y);
			setTrackerThreshold(profile.trackerThreshold_xz, profile.trackerThreshold_y);
			setUseTrackers(profile.useTrackers || profile.disableHMD);
			setDisableHMD(profile.disableHMD);
			setHandJogThreshold(profile.handJogThreshold);
			setHandRunThreshold(profile.handRunThreshold);
			setScaleTouchWithSwing(profile.scaleTouchWithSwing);
			setStepTime(profile.stepTime);
			setAccuracyButton(profile.useAccuracyButton);
			setAccuracyButtonAsToggle(profile.useButtonAsToggle);
			setAccuracyButtonFlip(profile.flipButtonUse);
			//setHMDPitchDown(profile.hmdPitchDown);
			//setHMDPitchUp(profile.hmdPitchUp);
			setWalkTouch(profile.walkTouch);
			setJogTouch(profile.jogTouch);
			setRunTouch(profile.runTouch);
		}
	}

	void WalkInPlaceTabController::deleteWalkInPlaceProfile(unsigned index) {
		if (index < walkInPlaceProfiles.size()) {
			auto pos = walkInPlaceProfiles.begin() + index;
			walkInPlaceProfiles.erase(pos);
			saveWalkInPlaceProfiles();
			OverlayController::appSettings()->sync();
			emit walkInPlaceProfilesChanged();
		}
	}

	void WalkInPlaceTabController::enableStepDetection(bool enable) {
		stepDetectEnabled = enable;
		_controllerDeviceIds[0] = -1;
		_controllerDeviceIds[1] = -1;
	}

	void WalkInPlaceTabController::enableBeta(bool enable) {
		betaEnabled = enable;
	}

	void WalkInPlaceTabController::setStepTime(double value) {
		_stepIntegrateStepLimit = (value * 1000);
	}

	void WalkInPlaceTabController::setWalkTouch(float value) {
		walkTouch = value;
	}

	void WalkInPlaceTabController::setJogTouch(float value) {
		jogTouch = value;
	}

	void WalkInPlaceTabController::setRunTouch(float value) {
		runTouch = value;
	}

	void WalkInPlaceTabController::setHMDThreshold(float xz, float y) {
		_hmdThreshold.v[0] = xz;
		_hmdThreshold.v[1] = y;
		_hmdThreshold.v[2] = xz;
	}

	void WalkInPlaceTabController::setTrackerThreshold(float xz, float y) {
		_trackerThreshold.v[0] = xz;
		_trackerThreshold.v[1] = y;
		_trackerThreshold.v[2] = xz;
	}

	void WalkInPlaceTabController::setUseTrackers(bool value) {
		useTrackers = value;
	}

	void WalkInPlaceTabController::setDisableHMD(bool value) {
		disableHMD = value;
		if (disableHMD && !useTrackers) {
			useTrackers = true;
		}
	}

	void WalkInPlaceTabController::setAccuracyButton(int buttonId) {
		useAccuracyButton = buttonId;
		switch (buttonId) {
		case 0:
			g_AccuracyButton = vr::EVRButtonId::k_EButton_Grip;
			g_accuracyButtonWithTouch = false;
			break;
		case 1:
			g_AccuracyButton = vr::EVRButtonId::k_EButton_Axis0;
			g_accuracyButtonWithTouch = true;
			break;
		case 2:
			g_AccuracyButton = vr::EVRButtonId::k_EButton_A;
			g_accuracyButtonWithTouch = false;
			break;
		case 3:
			g_AccuracyButton = vr::EVRButtonId::k_EButton_Axis1;
			g_accuracyButtonWithTouch = false;
			break;
		case 4:
			g_AccuracyButton = vr::EVRButtonId::k_EButton_ApplicationMenu;
			g_accuracyButtonWithTouch = false;
			break;
		case 5:
			g_AccuracyButton = -1;
			break;
		default:
			g_AccuracyButton = -1;
			break;
		}
	}

	void WalkInPlaceTabController::updateAccuracyButtonState(uint32_t deviceId, bool firstController) {
		if (deviceId >= 0 && (buttonControlSelect >= 2 || _controllerDeviceIds[buttonControlSelect] == deviceId)) {
			vr::VRControllerState_t state;
			vr::VRSystem()->GetControllerState(deviceId, &state, sizeof(state));
			//LOG(INFO) << "Check accuracy button : " << deviceId << " : " << g_AccuracyButton << " : " << state.ulButtonPressed << " : " << vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger);
			//LOG(INFO) << "current button : " << state.ulButtonPressed;
			bool isHoldingButton = false;
			switch (g_AccuracyButton) {
			case vr::EVRButtonId::k_EButton_Grip:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					isHoldingButton = true;
				}
				break;
			case vr::EVRButtonId::k_EButton_Axis0:
				if (state.ulButtonTouched& vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					isHoldingButton = true;
				}
				break;
			case vr::EVRButtonId::k_EButton_A:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_A)) {
					isHoldingButton = true;
				}
				break;
			case vr::EVRButtonId::k_EButton_Axis1:
				if (state.ulButtonTouched& vr::ButtonMaskFromId(vr::k_EButton_Axis1)) {
					isHoldingButton = true;
				}
				break;
			case vr::EVRButtonId::k_EButton_ApplicationMenu:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					isHoldingButton = true;
				}
				break;
			default:
				break;
			}
			if ((buttonControlSelect == 0 && firstController && isHoldingButton) ||
				(buttonControlSelect == 1 && !firstController && isHoldingButton) ||
				(buttonControlSelect >= 2 && isHoldingButton)) {
				g_isHoldingAccuracyButton = true;
			}
			if ((buttonControlSelect == 0 && firstController && !isHoldingButton) ||
				(buttonControlSelect == 1 && !firstController && !isHoldingButton)) {
				g_isHoldingAccuracyButton = false;
			}
		}
	}

	void WalkInPlaceTabController::setAccuracyButtonAsToggle(bool val) {
		useButtonAsToggle = val;
	}

	void WalkInPlaceTabController::setAccuracyButtonFlip(bool val) {
		flipButtonUse = val;
	}

	void WalkInPlaceTabController::setHandWalkThreshold(float walkThreshold) {
		handWalkThreshold = walkThreshold;
	}

	void WalkInPlaceTabController::setHandJogThreshold(float jogThreshold) {
		handJogThreshold = jogThreshold;
	}

	void WalkInPlaceTabController::setHandRunThreshold(float runThreshold) {
		handRunThreshold = runThreshold;
	}

	void WalkInPlaceTabController::setScaleTouchWithSwing(bool val) {
		scaleSpeedWithSwing = val;
	}

	void WalkInPlaceTabController::setGameStepType(int type) {
		gameType = type;
	}

	void WalkInPlaceTabController::setHMDType(int type) {
		hmdType = type;
	}

	void WalkInPlaceTabController::setControlSelect(int control) {
		controlSelect = control;
		if (control < 2) {
			_controlUsedID = _controllerDeviceIds[control];
			if (!identifyControlTimerSet && _controlUsedID >= 0) {
				identifyControlLastTime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				controlSelectOverlayHandle = 999;
				for (int d = 0; d < deviceInfos.size(); d++) {
					if (deviceInfos[d]->openvrId == _controlUsedID) {
						controlSelectOverlayHandle = d;
					}
				}
				try {
					if (vive_controller_model_index < 0) {
						int model_count = vr::VRRenderModels()->GetRenderModelCount();
						for (int model_index = 0; model_index < model_count; model_index++) {
							char buffer[vr::k_unMaxPropertyStringSize];
							vr::VRRenderModels()->GetRenderModelName(model_index, buffer, vr::k_unMaxPropertyStringSize);
							if ((std::string(buffer).compare("vr_controller_vive_1_5")) == 0) {
								vive_controller_model_index = model_index;
								break;
							}
						}
					}
				}
				catch (std::exception& e) {
					LOG(INFO) << "Exception caught while finding vive controller model: " << e.what();
				}
				if (vive_controller_model_index < 0) {
					vive_controller_model_index = 24;
				}
				setDeviceRenderModel(controlSelectOverlayHandle, vive_controller_model_index, 0, 1, 0, 1.1, 1.1, 1.1);
			}
		}
	}

	void WalkInPlaceTabController::setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex, float r, float g, float b, float sx, float sy, float sz) {
		if (deviceIndex < deviceInfos.size()) {
			try {
				if (renderModelIndex == 0) {
					if (deviceInfos[deviceIndex]->renderModelOverlay != vr::k_ulOverlayHandleInvalid) {
						vr::VROverlay()->DestroyOverlay(deviceInfos[deviceIndex]->renderModelOverlay);
						deviceInfos[deviceIndex]->renderModelOverlay = vr::k_ulOverlayHandleInvalid;
					}
				}
				else {
					vr::VROverlayHandle_t overlayHandle = deviceInfos[deviceIndex]->renderModelOverlay;
					if (overlayHandle == vr::k_ulOverlayHandleInvalid) {
						std::string overlayName = std::string("RenderModelOverlay_") + std::string(deviceInfos[deviceIndex]->serial);
						auto oerror = vr::VROverlay()->CreateOverlay(overlayName.c_str(), overlayName.c_str(), &overlayHandle);
						if (oerror == vr::VROverlayError_None) {
							overlayHandle = deviceInfos[deviceIndex]->renderModelOverlay = overlayHandle;
						}
						else {
							LOG(INFO) << "Could not create render model overlay: " << vr::VROverlay()->GetOverlayErrorNameFromEnum(oerror);
						}
					}
					if (overlayHandle != vr::k_ulOverlayHandleInvalid) {
						std::string texturePath = QApplication::applicationDirPath().toStdString() + "\\res\\transparent.png";
						if (QFile::exists(QString::fromStdString(texturePath))) {
							vr::VROverlay()->SetOverlayFromFile(overlayHandle, texturePath.c_str());
							char buffer[vr::k_unMaxPropertyStringSize];
							vr::VRRenderModels()->GetRenderModelName(renderModelIndex, buffer, vr::k_unMaxPropertyStringSize);
							vr::VROverlay()->SetOverlayRenderModel(overlayHandle, buffer, nullptr);
							vr::HmdMatrix34_t trans = {
								sx, 0.0f, 0.0f, 0.0f,
								0.0f, sy, 0.0f, 0.0f,
								0.0f, 0.0f, sz, 0.0f
							};
							vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(overlayHandle, deviceInfos[deviceIndex]->openvrId, &trans);
							vr::VROverlay()->ShowOverlay(overlayHandle);
							vr::VROverlay()->SetOverlayColor(overlayHandle, r, g, b);
							identifyControlTimerSet = true;
						}
						else {
							LOG(INFO) << "Could not find texture \"" << texturePath << "\"";
						}
					}
					//LOG(INFO) << "Successfully created control select Overlay for device: " << deviceInfos[deviceIndex]->openvrId << " Index: " << deviceIndex;
				}
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while updating control select overlay: " << e.what();
			}
		}
	}

	void WalkInPlaceTabController::setAccuracyButtonControlSelect(int control) {
		buttonControlSelect = control;
		if (control < 2) {
			if (!identifyControlTimerSet) {
				identifyControlTimerSet = true;
				identifyControlLastTime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				controlSelectOverlayHandle = 999;
				for (int d = 0; d < deviceInfos.size(); d++) {
					if (deviceInfos[d]->openvrId == _controllerDeviceIds[control]) {
						controlSelectOverlayHandle = d;
					}
				}
				try {
					if (vive_controller_model_index < 0) {
						int model_count = vr::VRRenderModels()->GetRenderModelCount();
						for (int model_index = 0; model_index < model_count; model_index++) {
							char buffer[vr::k_unMaxPropertyStringSize];
							vr::VRRenderModels()->GetRenderModelName(model_index, buffer, vr::k_unMaxPropertyStringSize);
							if ((std::string(buffer).compare("vr_controller_vive_1_5")) == 0) {
								vive_controller_model_index = model_index;
								break;
							}
						}
					}
				}
				catch (std::exception& e) {
					LOG(INFO) << "Exception caught while finding vive controller model: " << e.what();
				}
				if (vive_controller_model_index < 0) {
					vive_controller_model_index = 24;
				}
				setDeviceRenderModel(controlSelectOverlayHandle, vive_controller_model_index, 1, 0.6, 0, 1.1, 1.1, 1.1);
			}
		}
	}

	void WalkInPlaceTabController::applyStepPoseDetect() {
		double deltatime = 1.0 / 90.0 * 1000;
		auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		double tdiff = ((double)(now - _timeLastTick));
		//LOG(INFO) << "DT: " << tdiff;
		if (tdiff >= deltatime) {
			if (g_AccuracyButton >= 0 && _controllerDeviceIds[0] >= 0 && _controllerDeviceIds[1] >= 0) {
				g_isHoldingAccuracyButton = false;
				updateAccuracyButtonState(_controllerDeviceIds[0], true);
				updateAccuracyButtonState(_controllerDeviceIds[1], false);
				/*for (uint32_t i = 0; i < vr::k_unControllerStateAxisCount; i++) {
				vr::ETrackedDeviceProperty prop = (vr::ETrackedDeviceProperty)(vr::Prop_Axis0Type_Int32 + i);
				vr::EVRControllerAxisType type = (vr::EVRControllerAxisType)vr::VRSystem()->GetInt32TrackedDeviceProperty(_controllerDeviceIds[0], prop);
				LOG(INFO) << "Axis " << i << " type: " << vr::VRSystem()->GetControllerAxisTypeNameFromEnum(type) << endl;
				}*/
			}
		}
		bool moveButtonCheck = accuracyButtonOnOrDisabled();
		if (tdiff >= deltatime) {
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
			if (!_stepPoseDetected) {
				bool firstController = true;
				for (auto info : deviceInfos) {
					if (latestDevicePoses[info->openvrId].bPoseIsValid) {
						vr::ETrackedDeviceClass deviceClass = vr::VRSystem()->GetTrackedDeviceClass(info->openvrId);
						if (!disableHMD && deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {

							//LOG(INFO) << "HMD Rot: " << roll << "," << yaw << "," << pitch;


							vr::HmdVector3d_t poseWorldVel;// = vrmath::quaternionRotateVector(pose.qWorldFromDriverRotation, tmpConj, pose.vecVelocity, true);

							if (hmdType != 1) {
								auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
								double tvelDiff = ((double)(now - _velStepTime)) / 1000.0;
								auto m = latestDevicePoses[info->openvrId].mDeviceToAbsoluteTracking.m;

								poseWorldVel.v[0] = (m[0][3] - lastHmdPos.v[0]) / tvelDiff;
								poseWorldVel.v[1] = (m[1][3] - lastHmdPos.v[1]) / tvelDiff;
								poseWorldVel.v[2] = (m[2][3] - lastHmdPos.v[2]) / tvelDiff;

								hmdVel.v[0] = poseWorldVel.v[0];
								hmdVel.v[1] = poseWorldVel.v[1];
								hmdVel.v[2] = poseWorldVel.v[2];

								lastHmdPos.v[0] = m[0][3];
								lastHmdPos.v[1] = m[1][3];
								lastHmdPos.v[2] = m[2][3];

								_velStepTime = now;
							}
							else {
								poseWorldVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
								poseWorldVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
								poseWorldVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];
							}

							//LOG(INFO) << "HMD Step: " << poseWorldVel.v[0] << "," << poseWorldVel.v[1] << "," << poseWorldVel.v[2];
							//LOG(INFO) << "HMD POS: " << pose.vecPosition[0] << " " << pose.vecPosition[1] << " " << pose.vecPosition[2];

							if (upAndDownStepCheck(poseWorldVel, _hmdThreshold, roll, pitch)) {

								vr::HmdQuaternion_t qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[info->openvrId].mDeviceToAbsoluteTracking);

								vr::HmdVector3d_t forward = { 0,0,-1 };
								vr::HmdVector3d_t right = { 1,0,0 };
								vr::HmdVector3d_t pitchDir = vrmath::quaternionRotateVector(qRotation, forward);
								vr::HmdVector3d_t rollDir = vrmath::quaternionRotateVector(qRotation, right);

								pitch = (180 * std::asin(pitchDir.v[1])) / M_PI;
								roll = (180 * std::asin(rollDir.v[1])) / M_PI;

								if (!betaEnabled) {
									peaksCount = 1;
								}
								else {
									if (peaksCount == 0) {
										hmdLastYVel = poseWorldVel.v[1];
										peaksCount++;
										_timeLastStepPeak = now;
									}
									else {
										int velsign = poseWorldVel.v[1] > 0 ? 1 : -1;
										int hmdsign = hmdLastYVel > 0 ? 1 : -1;
										if (velsign != hmdsign) {
											peaksCount++;
											_timeLastStepPeak = now;
											hmdLastYVel = poseWorldVel.v[1];
										}
										else {
											if ((now - _timeLastStepPeak) > _stepFrequencyMin) {
												peaksCount = 0;
											}
										}
									}
								}
							}
							else {
								if (!betaEnabled) {
									peaksCount = 0;
									if ((now - _timeLastStepPeak) > _stepFrequencyMin) {
										trackerStepDetected = false;
									}
								}
								else {
									if ((now - _timeLastStepPeak) > _stepFrequencyMin) {
										peaksCount = 0;
										trackerStepDetected = false;
									}
								}
							}
						}
						else if (useTrackers && deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker) {

							vr::HmdVector3d_t poseWorldVel;// = vrmath::quaternionRotateVector(pose.qWorldFromDriverRotation, tmpConj, pose.vecVelocity, true);
							poseWorldVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
							poseWorldVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
							poseWorldVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];

							if (upAndDownStepCheck(poseWorldVel, _trackerThreshold, 0, 0)) {
								trackerStepDetected = true;
								_timeLastTrackerStep = now;
							}
						}
						else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
							if (_controllerDeviceIds[0] < 0) {
								_controllerDeviceIds[0] = info->openvrId;
							}
							else if (_controllerDeviceIds[1] < 0) {
								_controllerDeviceIds[1] = info->openvrId;
							}
							if (firstController) {
								firstController = false;
								if (controlSelect == 0 && _controlUsedID != info->openvrId) {
									_controlUsedID = info->openvrId;
									LOG(INFO) << "Set Main Controller to device : " << _controllerDeviceIds[0];
								}
							}
							else {
								if (controlSelect != 0 && _controlUsedID != info->openvrId) {
									_controlUsedID = info->openvrId;
									LOG(INFO) << "Set Main Controller to device : " << _controllerDeviceIds[1];
								}
							}
						}
						//else if ( deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
						//	LOG(INFO) << "CONTROLLER VEL: " << info->openvrId << pose.vecVelocity[0] << "," << pose.vecVelocity[1]  << "," << pose.vecVelocity[2];
						/*if (_controllerDeviceIds[0] >= 0 && _controllerDeviceIds[1] >= 0) {
						//check if first controller is running / jogging
						float * hand1Vel = latestDevicePoses[_controllerDeviceIds[0]].vVelocity.v;
						float * hand2Vel = latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v;
						bool isRunning = isRunningStep(hand1Vel);
						if (!isRunning) {
						if (avgContYVel <= 0) {
						avgContYVel = (std::fabs(hand1Vel[1]) + std::fabs(hand2Vel[1])) / 2.0;
						}
						else {
						avgContYVel = (avgContYVel + ((std::fabs(hand1Vel[1]) + std::fabs(hand2Vel[1])) / 2.0) / 2.0);
						}
						}
						}*/
						//}
					}
				}
				trackerStepDetected = trackerStepDetected || !useTrackers;
				if (!disableHMD) {
					if (!betaEnabled) {
						if (peaksCount >= 1 && (trackerStepDetected)) {
							//&& _openvrDeviceStepPoseTracker[1] != 0 && _openvrDeviceStepPoseTracker[2] != 0 ) {
							//&& (_openvrDeviceStepPoseTracker[1] != _openvrDeviceStepPoseTracker[2])) {
							//this->setStepPoseDetected(true);
							_stepPoseDetected = true;
							_hasUnTouchedStepAxis = 2;
						}
					}
					else {
						if (peaksCount >= stepPeaksToStart && (trackerStepDetected)) {
							_stepPoseDetected = true;
							_hasUnTouchedStepAxis = 2;
						}
					}
				}
				else if (trackerStepDetected && useTrackers) {
					_stepPoseDetected = true;
					_hasUnTouchedStepAxis = 2;
				}
				if (!_stepPoseDetected && _hasUnTouchedStepAxis < 6) {
					int deviceId = _controlUsedID;
					if (_controlUsedID < 0) {
						deviceId = _controllerDeviceIds[0];
					}
					if (gameType == 1 || gameType == 2 || gameType == 3 || gameType == 4) {
						if (_stepIntegrateSteps >= (_stepIntegrateStepLimit / 2.0)) {
							vr::VRControllerAxis_t axisState;
							axisState.x = 0;
							axisState.y = (walkTouch)*(_stepIntegrateSteps / (_stepIntegrateStepLimit / 2.0));
							applyAxisMovement(deviceId, axisState);
							_stepIntegrateSteps += tdiff;
							_hasUnTouchedStepAxis = 1;
						}
						else {
							stopMovement(deviceId);
							_hasUnTouchedStepAxis++;
						}
					}
					else {
						stopMovement(deviceId);
						_hasUnTouchedStepAxis++;
					}
					_teleportUnpressed = true;
				}
			}
			if (!_stepPoseDetected) {
				_timeLastTick = now;
			}
		}
		_stepPoseDetected = _stepPoseDetected && moveButtonCheck;
		if (_stepPoseDetected) {
			bool isWalking = false;
			bool isJogging = false;
			bool isRunning = false;
			bool axisStateChange = false;
			bool oneTrackerStepping = false;
			vr::VRControllerAxis_t axisState;
			float * hand1Vel;
			float * hand2Vel;
			if (tdiff >= deltatime) {
				for (auto info : deviceInfos) {
					vr::ETrackedDeviceClass deviceClass = vr::VRSystem()->GetTrackedDeviceClass(info->openvrId);
					if (!disableHMD && deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {
						double roll = 0;
						double pitch = 0;

						vr::HmdVector3d_t poseWorldVel;// = vrmath::quaternionRotateVector(pose.qWorldFromDriverRotation, tmpConj, pose.vecVelocity, true);

						if (hmdType != 1) {
							auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							double tvelDiff = ((double)(now - _velStepTime)) / 1000.0;
							if (tvelDiff > 0.0) {
								auto m = latestDevicePoses[info->openvrId].mDeviceToAbsoluteTracking.m;

								poseWorldVel.v[0] = (m[0][3] - lastHmdPos.v[0]) / tvelDiff;
								poseWorldVel.v[1] = (m[1][3] - lastHmdPos.v[1]) / tvelDiff;
								poseWorldVel.v[2] = (m[2][3] - lastHmdPos.v[2]) / tvelDiff;

								hmdVel.v[0] = poseWorldVel.v[0];
								hmdVel.v[1] = poseWorldVel.v[1];
								hmdVel.v[2] = poseWorldVel.v[2];

								lastHmdPos.v[0] = m[0][3];
								lastHmdPos.v[1] = m[1][3];
								lastHmdPos.v[2] = m[2][3];

								_velStepTime = now;
							}
						}
						else {
							poseWorldVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
							poseWorldVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
							poseWorldVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];
						}

						//LOG(INFO) << "HMD In Step: " << poseWorldVel.v[0] << "," << poseWorldVel.v[1] << "," << poseWorldVel.v[2];

						if (!betaEnabled) {
							if (upAndDownStepCheck(poseWorldVel, _hmdThreshold, roll, pitch)) {
								_stepIntegrateSteps = 0;
								isWalking = true;
								int velsign = poseWorldVel.v[1] > 0 ? 1 : -1;
								int hmdsign = hmdLastYVel > 0 ? 1 : -1;
								if (velsign != hmdsign) {
									_timeLastStepPeak = now;
									hmdLastYVel = poseWorldVel.v[1];
								}
							}
						}
						/*else if (isShakingHead(poseWorldVel, _hmdThreshold))
						{
						_stepIntegrateSteps = 9999;
						//LOG(INFO) << "HMD Rot: " << roll << "," << pitch;
						}*/
					}
					else if (useTrackers && deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker) {

						vr::HmdVector3d_t poseWorldVel;
						poseWorldVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
						poseWorldVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
						poseWorldVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];

						if (upAndDownStepCheck(poseWorldVel, _trackerThreshold, 0, 0)) {
							trackerStepDetected = true;
							oneTrackerStepping = true;
							_timeLastTrackerStep = now;
							if (disableHMD && useTrackers) {
								_stepIntegrateSteps = 0;
								isWalking = true;
							}
						}
					}
					//else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
					//}
				}
				if (useTrackers) {
					if (!disableHMD) {
						if ((!oneTrackerStepping && (now - _timeLastTrackerStep) > _stepIntegrateStepLimit * 3)) {
							trackerStepDetected = false;
							isWalking = false;
						}
					}
					else {
						if ((!oneTrackerStepping && (now - _timeLastTrackerStep) > _stepIntegrateStepLimit)) {
							trackerStepDetected = false;
							isWalking = false;
						}
					}
				}
				if (_controllerDeviceIds[0] >= 0 && _controllerDeviceIds[1] >= 0) {
					//check if first controller is running / jogging
					hand1Vel = latestDevicePoses[_controllerDeviceIds[0]].vVelocity.v;
					hand2Vel = latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v;
					isRunning = isRunningStep(hand1Vel);
					if (isRunning) {
						_runIntegrateSteps = 0;
					}
					else {
						isRunning = g_runPoseDetected;
					}
					if (!isRunning) {
						isJogging = isJoggingStep(hand1Vel);
						if (isJogging) {
							_jogIntegrateSteps = 0;
						}
						else {
							isJogging = g_jogPoseDetected;
						}
					}
					if (!betaEnabled) {
						//check if second controller is running / jogging
						isRunning = isRunningStep(hand2Vel);
						if (isRunning) {
							_runIntegrateSteps = 0;
						}
						else {
							isRunning = g_runPoseDetected;
						}
						if (!isRunning) {
							isJogging = isJoggingStep(hand2Vel);
							if (isJogging) {
								_jogIntegrateSteps = 0;
							}
							else {
								isJogging = g_jogPoseDetected;
							}
						}
					}
					else {
						isRunning = isRunning && isRunningStep(hand2Vel);
						if (!isRunning) {
							isJogging = isJogging && isJoggingStep(hand2Vel);
						}
						if (isJogging || isRunning) {
							int cont1Sign = hand1Vel[1] < 0 ? -1 : 1;
							int cont2Sign = hand2Vel[1] < 0 ? -1 : 1;
							if (cont1Sign != cont2Sign) {
								int lastcont1sign = cont1LastYVel < 0 ? -1 : 1;
								int lastcont2sign = cont2LastYVel < 0 ? -1 : 1;
								if ((lastcont1sign != cont1Sign || lastcont2sign != cont2Sign)) {
									cont1LastYVel = hand1Vel[1];
									cont2LastYVel = hand2Vel[1];
								}
							}
							else {
								isJogging = false;
								isRunning = false;
							}
						}
					}
					if (!isRunning) {
						/*if (avgContYVel <= 0) {
						avgContYVel = (std::fabs(hand1Vel[1]) + std::fabs(hand2Vel[1])) / 2.0;
						}
						else {
						avgContYVel = (avgContYVel + ((std::fabs(hand1Vel[1]) + std::fabs(hand2Vel[1])) / 2.0) / 2.0);
						}*/
					}
					if (g_jogPoseDetected) {
						_jogIntegrateSteps += tdiff;
						if (_jogIntegrateSteps > _stepIntegrateStepLimit) {
							g_jogPoseDetected = false;
							_jogIntegrateSteps = 0.0;
							isJogging = false;
							axisStateChange = true;
						}
					}
					if (g_runPoseDetected) {
						_runIntegrateSteps += tdiff;
						if (_runIntegrateSteps > _stepIntegrateStepLimit) {
							g_runPoseDetected = false;
							_teleportUnpressed = false;
							_runIntegrateSteps = 0.0;
							isRunning = false;
							axisStateChange = true;
						}
					}
				}
				_timeLastTick = now;
				_stepIntegrateSteps += tdiff;
			}
			else {
				isWalking = _stepPoseDetected;
				isJogging = g_jogPoseDetected;
				isRunning = g_runPoseDetected;
				axisStateChange = true;
			}
			trackerStepDetected = trackerStepDetected || !useTrackers;
			if (!moveButtonCheck || (useTrackers && !trackerStepDetected) || _stepIntegrateSteps >= (_stepIntegrateStepLimit)) {
				_stepPoseDetected = false;
				trackerStepDetected = false;
				if (_hasUnTouchedStepAxis == 0) {
					_stepIntegrateSteps = 0.0;
				}
				_jogIntegrateSteps = 0.0;
				_runIntegrateSteps = 0.0;
				//avgContYVel = -1;
				peaksCount = 0;
				g_stepPoseDetected = false;
				g_jogPoseDetected = false;
				if (gameType != 9999) {
					g_runPoseDetected = false;
				}
				//stopMovement(_controlUsedID);
			}
			else {
				if (_controllerDeviceIds[0] >= 0 && _controllerDeviceIds[1] >= 0) {
					int deviceId = _controlUsedID;
					if (_controlUsedID < 0) {
						deviceId = _controllerDeviceIds[0];
					}
					if (gameType == 1 || gameType == 2 || gameType == 3 || gameType == 4) {
						axisStateChange = _hasUnTouchedStepAxis != 0;
						axisState.x = 0;
						axisState.y = walkTouch;
						axisStateChange = axisStateChange || (isRunning && !g_runPoseDetected);
						if (isRunning) {
							axisState.y = runTouch;
							g_runPoseDetected = true;
						}
						else if (isJogging) {
							//axisState.y = jogTouch;
							axisState.y = jogTouch; //getScaledTouch(jogTouch, runTouch, avgContYVel, handRunThreshold);
							axisStateChange = axisStateChange || (!g_jogPoseDetected);
							g_jogPoseDetected = true;
						}
						else {
							axisState.y = walkTouch; //getScaledTouch(walkTouch, jogTouch, avgContYVel, handJogThreshold);
						}
						if (true || axisStateChange) {
							if (axisState.y > 1) {
								axisState.y = 1;
							}
							try {
								if (!_teleportUnpressed) {
									stopClickMovement(deviceId);
								}
								applyAxisMovement(deviceId, axisState);
								_hasUnTouchedStepAxis = 0;
							}
							catch (std::exception& e) {
								//LOG(INFO) << "Exception caught while applying virtual step movement: " << e.what();
							}
						}
					}
					else if (gameType == 9999) {
						applyClickMovement(deviceId);
					}
					else if (gameType == 5) {
						applyGripMovement(deviceId);
					}
				}
				if (gameType == 6) {
					if (_teleportUnpressed) {
						INPUT input[2];
						input[0].type = INPUT_KEYBOARD;
						input[0].ki.wVk = 0;
						input[0].ki.wScan = MapVirtualKey(0x57, 0);
						input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
						input[0].ki.time = 0;
						input[0].ki.dwExtraInfo = 0;
						SendInput(1, input, sizeof(INPUT));

						_teleportUnpressed = false;
					}
				}
				else if (gameType == 7) {
					if (_teleportUnpressed) {
						INPUT input[2];
						input[0].type = INPUT_KEYBOARD;
						input[0].ki.wVk = 0;
						input[0].ki.wScan = MapVirtualKey(0x26, 0);
						input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
						input[0].ki.time = 0;
						input[0].ki.dwExtraInfo = 0;
						SendInput(1, input, sizeof(INPUT));
					}
					_teleportUnpressed = false;
				}
			}
		}
	}

	bool WalkInPlaceTabController::accuracyButtonOnOrDisabled() {
		return (g_AccuracyButton < 0
			|| ((g_isHoldingAccuracyButton && !flipButtonUse)
				|| (!g_isHoldingAccuracyButton && flipButtonUse)));
	}

	bool WalkInPlaceTabController::upAndDownStepCheck(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold, double roll, double pitch) {
		bool stepParams = ((std::fabs(vel.v[2]) < threshold.v[2]) &&
			(std::fabs(vel.v[0]) < threshold.v[0]) &&
			((vel.v[1] > threshold.v[1] || vel.v[1] < -1 * threshold.v[1])
				&& (std::fabs(vel.v[1]) > std::fabs(vel.v[0]) && std::fabs(vel.v[1]) > std::fabs(vel.v[2]))));
		return stepParams;
	}

	bool WalkInPlaceTabController::sideToSideStepCheck(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold) {
		bool stepParams = ((std::fabs(vel.v[2]) > threshold.v[1] && std::fabs(vel.v[2]) > threshold.v[2]) ||
			(std::fabs(vel.v[0]) > threshold.v[1] && std::fabs(vel.v[0]) > threshold.v[0]));
		return stepParams;
	}

	bool WalkInPlaceTabController::isJoggingStep(float * vel) {
		float magVel = std::fabs(vel[1]);// (std::abs(vel[0]) + std::abs(vel[1]) + std::abs(vel[2]));
		return (std::fabs(vel[1]) > std::fabs(vel[0]) && std::fabs(vel[1]) > std::fabs(vel[2]))
			&& (magVel > handJogThreshold);
	}

	bool WalkInPlaceTabController::isRunningStep(float * vel) {
		float magVel = std::fabs(vel[1]);// (std::abs(vel[0]) + std::abs(vel[1]) + std::abs(vel[2]));
		return (std::fabs(vel[1]) > std::fabs(vel[0]) && std::fabs(vel[1]) > std::fabs(vel[2]))
			&& (magVel > handRunThreshold);
	}

	float WalkInPlaceTabController::getScaledTouch(float minTouch, float maxTouch, float avgVel, float maxVel) {
		float scaledTouch = maxTouch;
		if (avgVel < maxVel) {
			scaledTouch = (avgVel / maxVel);
		}
		if (scaledTouch < minTouch) {
			scaledTouch = minTouch;
		}
		return scaledTouch > 1 ? 1 : scaledTouch;
	}


	/********************************* DRIVER DETERMINED SPECIFIC CODE ***************************/
	/*********************************************************************************************/
	/*********************************************************************************************/


	void WalkInPlaceTabController::stopMovement(uint32_t deviceId) {
		if (gameType == 1 || gameType == 2) {
			vr::VRControllerAxis_t axisState;
			axisState.x = 0;
			axisState.y = 0;
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				if (gameType != 2) {
					vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
				}
				vrinputemulator.openvrAxisEvent(deviceId, 0, axisState);
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_Axis0, 0.0);
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
			}
		}
		else if (gameType == 3 || gameType == 4) {
			vr::VRControllerAxis_t axisState;
			axisState.x = 0;
			axisState.y = 0;
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				if (gameType != 4) {
					vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis1, 0.0);
				}
				vrinputemulator.openvrAxisEvent(deviceId, 1, axisState);
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_Axis1, 0.0);
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
			}
		}
		else if (gameType == 5) {
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Grip, 0.0);
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while applying virtual telport movement: " << e.what();
			}
		}
		else if (gameType == 9999) {
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual teleport movement: " << e.what();
			}
		}
		else if (gameType == 6) {
			INPUT input[2];
			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wVk = 0;
			input[0].ki.wScan = MapVirtualKey(0x57, 0);
			input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
			input[0].ki.time = 0;
			input[0].ki.dwExtraInfo = 0;
			SendInput(2, input, sizeof(INPUT));
		}
		else if (gameType == 7) {
			INPUT input[2];
			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wVk = 0;
			input[0].ki.wScan = MapVirtualKey(0x26, 0);
			input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
			input[0].ki.time = 0;
			input[0].ki.dwExtraInfo = 0;
			SendInput(2, input, sizeof(INPUT));
		}
	}

	void WalkInPlaceTabController::stopClickMovement(uint32_t deviceId) {
		if (gameType == 1) {
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				if (gameType != 2) {
					vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
				}
				_teleportUnpressed = true;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
			}
		}
		else if (gameType == 3) {
			vr::VRControllerAxis_t axisState;
			axisState.x = 0;
			axisState.y = 0;
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				if (gameType != 4) {
					vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis1, 0.0);
				}
				_teleportUnpressed = true;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
			}
		}
	}

	void WalkInPlaceTabController::applyAxisMovement(uint32_t deviceId, vr::VRControllerAxis_t axisState) {
		vrinputemulator::VRInputEmulator vrinputemulator;
		vrinputemulator.connect();
		if (gameType == 1 || gameType == 2) {
		vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_Axis0, 0.0);
		vrinputemulator.openvrAxisEvent(deviceId, 0, axisState);
		if (gameType != 2) {
			if (g_runPoseDetected) {
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_Axis0, 0.0);
			}
			else {
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
			}
		}
		}
		else if (gameType == 3 || gameType == 4) {
			vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_Axis1, 0.0);
			vrinputemulator.openvrAxisEvent(deviceId, 1, axisState);
			if (gameType != 4) {
				if (g_runPoseDetected) {
					vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_Axis0, 0.0);
				}
				else {
					vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
				}
			}
		}
	}

	void WalkInPlaceTabController::applyClickMovement(uint32_t deviceId) {
		if (_teleportUnpressed) {
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_Axis0, 0.0);
				_teleportUnpressed = false;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while applying virtual telport movement: " << e.what();
			}
		}
		else {
			try {
				vrinputemulator::VRInputEmulator vrinputemulator;
				vrinputemulator.connect();
				vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
				_teleportUnpressed = true;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while resetting virtual telport movement: " << e.what();
			}
		}
		_hasUnTouchedStepAxis = 0;
	}


	void WalkInPlaceTabController::applyGripMovement(uint32_t deviceId) {
		try {
			vrinputemulator::VRInputEmulator vrinputemulator;
			vrinputemulator.connect();
			vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_Grip, 0.0);
		}
		catch (std::exception& e) {
			//LOG(INFO) << "Exception caught while applying virtual telport movement: " << e.what();
		}
		_hasUnTouchedStepAxis = 0;
	}


} // namespace walkinplace
