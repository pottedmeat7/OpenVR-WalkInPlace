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

	int WalkInPlaceTabController::getControlSelect() {
		return controlSelect;
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

	float WalkInPlaceTabController::getHandWalkThreshold() {
		return handWalkThreshold;
	}

	float WalkInPlaceTabController::getHandJogThreshold() {
		return handJogThreshold;
	}

	float WalkInPlaceTabController::getHandRunThreshold() {
		return handRunThreshold;
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
		for (auto info : deviceInfos) {
			if (latestDevicePoses[info->openvrId].bPoseIsValid) {
				if (info->deviceClass == vr::TrackedDeviceClass_HMD) {
					hmdVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
					hmdVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
					hmdVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];

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
		//LOG(INFO) << "HMD VALS: " << hmdVel.v[0] << "," << hmdVel.v[1] << "," << hmdVel.v[2];
		vals.push_back(_stepPoseDetected ? 1 : 0);
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
			entry.controlSelect = settings->value("controlSelect", 2).toInt();
			entry.hmdThreshold_y = settings->value("hmdthreshold_y", 0.27).toFloat();
			entry.hmdThreshold_xz = settings->value("hmdthreshold_xz", 0.27).toFloat();
			entry.handJogThreshold = settings->value("handJog", 0.35).toFloat();
			entry.handRunThreshold = settings->value("handRun", 1.7).toFloat();
			entry.stepTime = settings->value("stepTime", 0.7).toDouble();
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
			settings->setValue("controlSelect", p.controlSelect);
			settings->setValue("hmdthreshold_y", p.hmdThreshold_y);
			settings->setValue("hmdthreshold_xz", p.hmdThreshold_xz);
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
		profile->controlSelect = controlSelect;
		profile->hmdThreshold_y = _hmdThreshold.v[1];
		profile->hmdThreshold_xz = _hmdThreshold.v[0];
		profile->handJogThreshold = handJogThreshold;
		profile->handRunThreshold = handRunThreshold;
		profile->stepTime = (_stepIntegrateStepLimit / 1000);
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
			controlSelect = profile.controlSelect;
			_hmdThreshold.v[0] = profile.hmdThreshold_xz;
			_hmdThreshold.v[1] = profile.hmdThreshold_y;
			_hmdThreshold.v[2] = profile.hmdThreshold_xz;
			handJogThreshold = profile.handJogThreshold;
			handRunThreshold = profile.handRunThreshold;
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
			setControlSelect(profile.controlSelect);
			setHMDThreshold(profile.hmdThreshold_xz, profile.hmdThreshold_y);
			setHandJogThreshold(profile.handJogThreshold);
			setHandRunThreshold(profile.handRunThreshold);
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

	void WalkInPlaceTabController::setGameStepType(int type) {
		gameType = type;
	}

	void WalkInPlaceTabController::setControlSelect(int control) {
		controlSelect = control;
		_controlUsedID = _controllerDeviceIds[control];
	}

	void WalkInPlaceTabController::applyStepPoseDetect() {
		double deltatime = 1.0 / 90.0 * 1000;
		auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		double tdiff = ((double)(now - _timeLastTick));
		//LOG(INFO) << "DT: " << tdiff;
		if (tdiff >= deltatime) {
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
			if (g_AccuracyButton >= 0 && _controllerDeviceIds[0] >= 0 && _controllerDeviceIds[1] >= 0) {
				updateAccuracyButtonState(_controllerDeviceIds[0], true);
				updateAccuracyButtonState(_controllerDeviceIds[1], false);
			}
			if (!_stepPoseDetected) {
				bool firstController = true;
				for (auto info : deviceInfos) {
					if (latestDevicePoses[info->openvrId].bPoseIsValid) {
						vr::ETrackedDeviceClass deviceClass = vr::VRSystem()->GetTrackedDeviceClass(info->openvrId);
						bool isHMD = deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD;
						if (isHMD) {
							//LOG(INFO) << "Detecting step";
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
							}*/
							double roll = 0;
							double pitch = 0;

							//double t3 = +2.0 * (poseWorldRot.w * poseWorldRot.y + poseWorldRot.x * poseWorldRot.z);
							//double t4 = +1.0 - 2.0 * (zsqr + poseWorldRot.y * poseWorldRot.y);
							//double yaw = (180 * std::atan2(t3, t4)) / M_PI;

							//LOG(INFO) << "HMD Rot: " << roll << "," << yaw << "," << pitch;


							vr::HmdVector3d_t poseWorldVel;// = vrmath::quaternionRotateVector(pose.qWorldFromDriverRotation, tmpConj, pose.vecVelocity, true);
							poseWorldVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
							poseWorldVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
							poseWorldVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];

							//LOG(INFO) << "HMD Step: " << poseWorldVel.v[0] << "," << poseWorldVel.v[1] << "," << poseWorldVel.v[2];
							//LOG(INFO) << "HMD POS: " << pose.vecPosition[0] << " " << pose.vecPosition[1] << " " << pose.vecPosition[2];

							if (isTakingStep(poseWorldVel, _hmdThreshold, roll, pitch)) { //&& isStepingInPlace(pose.vecPosition) ) {
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
								}
								else {
									if ((now - _timeLastStepPeak) > _stepFrequencyMin) {
										peaksCount = 0;
									}

								}
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
									LOG(INFO) << "Set Secondary Controller to device : " << _controllerDeviceIds[1];
								}
							}
						}
						//else if ( deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
						//	LOG(INFO) << "CONTROLLER VEL: " << info->openvrId << pose.vecVelocity[0] << "," << pose.vecVelocity[1]  << "," << pose.vecVelocity[2];
						//}
					}
				}
				if (!betaEnabled) {
					if (peaksCount == 1) {
						//&& _openvrDeviceStepPoseTracker[1] != 0 && _openvrDeviceStepPoseTracker[2] != 0 ) {
						//&& (_openvrDeviceStepPoseTracker[1] != _openvrDeviceStepPoseTracker[2])) {
						//this->setStepPoseDetected(true);
						_stepPoseDetected = true;
						_hasUnTouchedStepAxis = 2;
					}
				}
				else {
					if (peaksCount >= stepPeaksToStart) {
						_stepPoseDetected = true;
						_hasUnTouchedStepAxis = 2;
					}
				}
				if (!_stepPoseDetected && _hasUnTouchedStepAxis < 6) {
					int deviceId = _controlUsedID;
					if (_controlUsedID < 0) {
						deviceId = _controllerDeviceIds[0];
					}
					if (gameType != 3) {
						vr::VRControllerAxis_t axisState;
						axisState.x = 0;
						axisState.y = 0;
						try {
							LOG(INFO) << "stopping virtual step movement";
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
					else {
						try {
							vrinputemulator::VRInputEmulator vrinputemulator;
							vrinputemulator.connect();
							vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
						}
						catch (std::exception& e) {
							//LOG(INFO) << "Exception caught while stopping virtual teleport movement: " << e.what();
						}
					}
					_hasUnTouchedStepAxis++;
					_teleportUnpressed = true;
				}
			}
			if (_stepPoseDetected) {
				bool isWalking = false;
				bool isJogging = false;
				bool isRunning = false;
				bool axisStateChange = false;
				for (auto info : deviceInfos) {
					if (info->deviceClass == vr::TrackedDeviceClass_HMD) {// || info->deviceClass == vr::TrackedDeviceClass_Controller) {
						vr::ETrackedDeviceClass deviceClass = vr::VRSystem()->GetTrackedDeviceClass(info->openvrId);
						bool isHMD = deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD;
						if (isHMD) {
							double roll = 0;
							double pitch = 0;

							vr::HmdVector3d_t poseWorldVel;// = vrmath::quaternionRotateVector(pose.qWorldFromDriverRotation, tmpConj, pose.vecVelocity, true);
							poseWorldVel.v[0] = latestDevicePoses[info->openvrId].vVelocity.v[0];
							poseWorldVel.v[1] = latestDevicePoses[info->openvrId].vVelocity.v[1];
							poseWorldVel.v[2] = latestDevicePoses[info->openvrId].vVelocity.v[2];

							//LOG(INFO) << "HMD In Step: " << poseWorldVel.v[0] << "," << poseWorldVel.v[1] << "," << poseWorldVel.v[2];

							if (!betaEnabled) {
								if (isTakingStep(poseWorldVel, _hmdThreshold, roll, pitch)) {
									_stepIntegrateSteps = 0;
									_timeLastStepPeak = now;
									isWalking = true;
								}
							}
							else {
								if (isTakingStep(poseWorldVel, _hmdThreshold, roll, pitch)) {
									_stepIntegrateSteps = 0;
									isWalking = true;
									int velsign = poseWorldVel.v[1] > 0 ? 1 : -1;
									int hmdsign = hmdLastYVel > 0 ? 1 : -1;
									if (velsign != hmdsign) {
										_timeLastStepPeak = now;
										hmdLastYVel = poseWorldVel.v[1];
										if (peaksCount < stepPeaksFullSpeed) {
											peaksCount++;
											axisStateChange = true;
										}
									}
								}
								else {
									if ((now - _timeLastStepPeak) > _stepFrequencyMin) {
										if (peaksCount > 1) {
											peaksCount--;
											axisStateChange = true;
										}
									}
								}
							}
							/*else if (isShakingHead(poseWorldVel, _hmdThreshold))
							{
							_stepIntegrateSteps = 9999;
							//LOG(INFO) << "HMD Rot: " << roll << "," << pitch;
							}*/
						}
						//else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
						//}
					}
				}
				if (_controllerDeviceIds[0] >= 0 && _controllerDeviceIds[1] >= 0) {
					isRunning = isRunningStep(latestDevicePoses[_controllerDeviceIds[0]].vVelocity.v);
					if (!isRunning) {
						isJogging = isJoggingStep(latestDevicePoses[_controllerDeviceIds[0]].vVelocity.v);
					}
					if (!betaEnabled) {
						isRunning = isRunning || isRunningStep(latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v);
						if (!isRunning) {
							isJogging = isJogging || isJoggingStep(latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v);
						}
					}
					else {
						isRunning = isRunning && isRunningStep(latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v);
						if (!isRunning) {
							isJogging = isJogging && isJoggingStep(latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v);
						}
						if (isJogging || isRunning) {
							int cont1Sign = latestDevicePoses[_controllerDeviceIds[0]].vVelocity.v[1] < 0 ? -1 : 1;
							int cont2Sign = latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v[1] < 0 ? -1 : 1;
							if (cont1Sign != cont2Sign) {
								int lastcont1sign = cont1LastYVel < 0 ? -1 : 1;
								int lastcont2sign = cont2LastYVel < 0 ? -1 : 1;
								if ((lastcont1sign != cont1Sign || lastcont2sign != cont2Sign)) {
									cont1LastYVel = latestDevicePoses[_controllerDeviceIds[0]].vVelocity.v[1];
									cont2LastYVel = latestDevicePoses[_controllerDeviceIds[1]].vVelocity.v[1];
									if (peaksCount < stepPeaksFullSpeed) {
										peaksCount++;
										axisStateChange = true;
									}
								}
							}
							else {
								isJogging = false;
								isRunning = false;
							}
						}
					}
					int deviceId = _controlUsedID;
					if (_controlUsedID < 0) {
						deviceId = _controllerDeviceIds[0];
					}
					if (gameType == 1 || gameType == 2) {
						axisStateChange = _hasUnTouchedStepAxis != 0;
						if (isJogging) {
							_jogIntegrateSteps = 0;
						}
						if (isRunning) {
							_runIntegrateSteps = 0;
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
								_runIntegrateSteps = 0.0;
								isRunning = false;
								axisStateChange = true;
							}
						}
						vr::VRControllerAxis_t axisState;
						axisState.x = 0;
						axisState.y = walkTouch;
						axisStateChange = axisStateChange || (isRunning && !g_runPoseDetected);
						if (isRunning) {
							axisState.y = runTouch;
							g_runPoseDetected = true;
						}
						else if (isJogging) {
							axisState.y = jogTouch;
							axisStateChange = axisStateChange || (!g_jogPoseDetected);
							g_jogPoseDetected = true;
						}
						/*if (betaEnabled) {
							if (!isRunning && !isJogging && isWalking) {
								axisState.y = axisState.y * (((float)peaksCount) / stepPeaksFullSpeed);
								if (axisState.y < minTouch) {
									axisState.y = minTouch;
								}
							}
						}*/
						if (axisStateChange) {
							try {
								LOG(INFO) << "applying virtual step movement";
								vrinputemulator::VRInputEmulator vrinputemulator;
								vrinputemulator.connect();
								//vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_Axis0, 0.0);
								vrinputemulator.openvrAxisEvent(deviceId, 0, axisState);
								if (gameType != 2) {
									if (g_runPoseDetected) {
										vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_Axis0, 0.0);
									}
									else {
										vrinputemulator.openvrButtonEvent(vrinputemulator::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Axis0, 0.0);
									}
								}
								_hasUnTouchedStepAxis = 0;
							}
							catch (std::exception& e) {
								//LOG(INFO) << "Exception caught while applying virtual step movement: " << e.what();
							}
						}
					}
					else if (gameType == 3) {
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
				}
				if (gameType == 4) {
					INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x57, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(1, input, sizeof(INPUT));

					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x57, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(2, input, sizeof(INPUT));
				}
				else if (gameType == 5) {
					INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x26, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(1, input, sizeof(INPUT));

					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x26, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(2, input, sizeof(INPUT));
				}
				if (_stepIntegrateSteps >= (_stepIntegrateStepLimit)) {
					_stepPoseDetected = false;
					_stepIntegrateSteps = 0.0;
					_jogIntegrateSteps = 0.0;
					_runIntegrateSteps = 0.0;
					peaksCount = 0;
					g_stepPoseDetected = false;
					g_jogPoseDetected = false;
					if (gameType != 3) {
						g_runPoseDetected = false;
					}
					if (gameType == 4) {
						INPUT input[2];
						input[0].type = INPUT_KEYBOARD;
						input[0].ki.wVk = 0;
						input[0].ki.wScan = MapVirtualKey(0x57, 0);
						input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
						input[0].ki.time = 0;
						input[0].ki.dwExtraInfo = 0;
						SendInput(2, input, sizeof(INPUT));
					}
					else if (gameType == 5) {
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
				_stepIntegrateSteps += tdiff;
			}
			_timeLastTick = now;
		}
	}

	void WalkInPlaceTabController::updateAccuracyButtonState(uint32_t deviceId, bool firstController) {
		if (deviceId >= 0) {
			vr::VRControllerState_t state;
			vr::VRSystem()->GetControllerState(deviceId, &state, sizeof(state));
			//LOG(ERROR) << "Check accuracy button : " << deviceId << " : " << g_AccuracyButton << " : " << state.ulButtonPressed << " : " << vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger);
			if (firstController) {
				g_isHoldingAccuracyButton = false;
				g_isHoldingAccuracyButton1 = false;
				g_isHoldingAccuracyButton2 = false;
			}
			switch (g_AccuracyButton) {
			case vr::EVRButtonId::k_EButton_Grip:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_Grip)) {
					if (g_useButtonAsToggle) {
						if (g_buttonToggled) {
							if (firstController) {
								g_isHoldingAccuracyButton1 = !g_isHoldingAccuracyButton1;
							}
							else {
								g_isHoldingAccuracyButton2 = !g_isHoldingAccuracyButton2;
							}
							g_buttonToggled = false;
						}
					}
					else {
						if (firstController) {
							g_isHoldingAccuracyButton1 = true;
						}
						else {
							g_isHoldingAccuracyButton2 = true;
						}
					}
				}
				break;
			case vr::EVRButtonId::k_EButton_Axis0:
				if (state.ulButtonTouched& vr::ButtonMaskFromId(vr::k_EButton_Axis0)) {
					if (g_useButtonAsToggle) {
						if (g_buttonToggled) {
							if (firstController) {
								g_isHoldingAccuracyButton1 = !g_isHoldingAccuracyButton1;
							}
							else {
								g_isHoldingAccuracyButton2 = !g_isHoldingAccuracyButton2;
							}
							g_buttonToggled = false;
						}
					}
					else {
						if (firstController) {
							g_isHoldingAccuracyButton1 = true;
						}
						else {
							g_isHoldingAccuracyButton2 = true;
						}
					}
				}
				break;
			case vr::EVRButtonId::k_EButton_A:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_A)) {
					if (g_useButtonAsToggle) {
						if (g_buttonToggled) {
							if (firstController) {
								g_isHoldingAccuracyButton1 = !g_isHoldingAccuracyButton1;
							}
							else {
								g_isHoldingAccuracyButton2 = !g_isHoldingAccuracyButton2;
							}
							g_buttonToggled = false;
						}
					}
					else {
						if (firstController) {
							g_isHoldingAccuracyButton1 = true;
						}
						else {
							g_isHoldingAccuracyButton2 = true;
						}
					}
				}
				break;
			case vr::EVRButtonId::k_EButton_Axis1:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_Axis1)) {
					if (g_useButtonAsToggle) {
						if (g_buttonToggled) {
							if (firstController) {
								g_isHoldingAccuracyButton1 = !g_isHoldingAccuracyButton1;
							}
							else {
								g_isHoldingAccuracyButton2 = !g_isHoldingAccuracyButton2;
							}
							g_buttonToggled = false;
						}
					}
					else {
						if (firstController) {
							g_isHoldingAccuracyButton1 = true;
						}
						else {
							g_isHoldingAccuracyButton2 = true;
						}
					}
				}
				break;
			case vr::EVRButtonId::k_EButton_ApplicationMenu:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) {
					if (g_useButtonAsToggle) {
						if (g_buttonToggled) {
							if (firstController) {
								g_isHoldingAccuracyButton1 = !g_isHoldingAccuracyButton1;
							}
							else {
								g_isHoldingAccuracyButton2 = !g_isHoldingAccuracyButton2;
							}
							g_buttonToggled = false;
						}
					}
					else {
						if (firstController) {
							g_isHoldingAccuracyButton1 = true;
						}
						else {
							g_isHoldingAccuracyButton2 = true;
						}
					}
				}
				break;
			default:
				break;
			}
			if (!firstController && !g_isHoldingAccuracyButton1 && !g_isHoldingAccuracyButton2) {
				g_buttonToggled = true;
			}
			if (!firstController && (g_isHoldingAccuracyButton1 || g_isHoldingAccuracyButton2)) {
				g_isHoldingAccuracyButton = true;
			}
		}
	}

	bool WalkInPlaceTabController::isTakingStep(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold, double roll, double pitch) {
		bool stepParams = ((std::fabs(vel.v[2]) < threshold.v[2]) &&
			(std::fabs(vel.v[0]) < threshold.v[0]) &&
			((vel.v[1] > threshold.v[1] || vel.v[1] < -1 * threshold.v[1])
				&& (std::fabs(vel.v[1]) > std::fabs(vel.v[0]) && std::fabs(vel.v[1]) > std::fabs(vel.v[2]))));
		stepParams = stepParams && (g_AccuracyButton < 0
			|| ((g_isHoldingAccuracyButton && !flipButtonUse)
				|| (!g_isHoldingAccuracyButton && flipButtonUse)));
		return stepParams;
	}

	bool WalkInPlaceTabController::isShakingHead(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold) {
		bool stepParams = ((std::fabs(vel.v[2]) > threshold.v[1] && std::fabs(vel.v[2]) > threshold.v[2]) ||
			(std::fabs(vel.v[0]) > threshold.v[1] && std::fabs(vel.v[0]) > threshold.v[0]));
		return stepParams;
	}

	bool WalkInPlaceTabController::isStepingInPlace(float * pos) {
		/*vr::HmdVector3d_t currentDeviation = { 0.0, 0.0, 0.0 };
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
		));*/
		return false;
	}

	bool WalkInPlaceTabController::isJoggingStep(float * vel) {
		float magVel = std::abs(vel[1]);// (std::abs(vel[0]) + std::abs(vel[1]) + std::abs(vel[2]));
		return (std::abs(vel[1]) > std::abs(vel[0]) && std::abs(vel[1]) > std::abs(vel[2]))
			&& (magVel > handJogThreshold)
			&& (g_AccuracyButton < 0
				|| ((g_isHoldingAccuracyButton && !flipButtonUse)
					|| (!g_isHoldingAccuracyButton && flipButtonUse)));
	}

	bool WalkInPlaceTabController::isRunningStep(float * vel) {
		float magVel = std::abs(vel[1]);// (std::abs(vel[0]) + std::abs(vel[1]) + std::abs(vel[2]));
		return (std::abs(vel[1]) > std::abs(vel[0]) && std::abs(vel[1]) > std::abs(vel[2]))
			&& (magVel > handRunThreshold) && (g_AccuracyButton < 0
				|| ((g_isHoldingAccuracyButton && !flipButtonUse)
					|| (!g_isHoldingAccuracyButton && flipButtonUse)));
	}
} // namespace walkinplace
