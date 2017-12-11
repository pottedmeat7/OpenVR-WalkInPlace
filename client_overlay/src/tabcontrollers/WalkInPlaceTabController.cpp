#include "WalkInPlaceTabController.h"
#include <QQuickWindow>
#include <QApplication>
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
			vrwalkinplace.connect();
			for (uint32_t id = 0; id < vr::k_unMaxTrackedDeviceCount; ++id) {
				auto deviceClass = vr::VRSystem()->GetTrackedDeviceClass(id);
				if (deviceClass != vr::TrackedDeviceClass_Invalid) {
					if (deviceClass == vr::TrackedDeviceClass_Controller || deviceClass == vr::TrackedDeviceClass_GenericTracker) {
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


	void WalkInPlaceTabController::eventLoopTick(vr::TrackedDevicePose_t* devicePoses) {
		if (settingsUpdateCounter >= 50) {
			settingsUpdateCounter = 0;
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

							try {
								//vrwalkinplace::DeviceInfo info2;
								//vrwalkinplace.getDeviceInfo(info->openvrId, info2);
								//info->deviceMode = info2.deviceMode;
								info->deviceMode = 0;
							}
							catch (std::exception& e) {
								LOG(ERROR) << "Exception caught while getting device info: " << e.what();
							}

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

	float WalkInPlaceTabController::getStepIntSec() {
		return stepIntSec;
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

	int WalkInPlaceTabController::getHMDPitchDown() {
		return hmdPitchDown;
	}

	int WalkInPlaceTabController::getHMDPitchUp() {
		return hmdPitchUp;
	}

	float WalkInPlaceTabController::getHMDXZThreshold() {
		return hmdThreshold_xz;
	}

	float WalkInPlaceTabController::getHMDYThreshold() {
		return hmdThreshold_y;
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
			entry.hmdThreshold_y = settings->value("hmdthreshold_y", 0.1).toFloat();
			entry.hmdThreshold_xz = settings->value("hmdthreshold_xz", 0.09).toFloat();
			entry.handJogThreshold = settings->value("handJog", 0.35).toFloat();
			entry.handRunThreshold = settings->value("handRun", 1.7).toFloat();
			entry.useAccuracyButton = settings->value("useAccuracyButton", 0).toInt();
			entry.hmdPitchDown = settings->value("hmdPitchDown", 20).toInt();
			entry.hmdPitchUp = settings->value("hmdPitchUp", 15).toInt();
			entry.walkTouch = settings->value("walkTouch", 0.6).toFloat();
			entry.jogTouch = settings->value("jogTouch", 0.87).toFloat();
			entry.runTouch = settings->value("runTouch", 1).toFloat();
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
			settings->setValue("useAccuracyButton", p.useAccuracyButton);
			settings->setValue("hmdPitchDown", p.hmdPitchDown);
			settings->setValue("hmdPitchUp", p.hmdPitchUp);
			settings->setValue("walkTouch", p.walkTouch);
			settings->setValue("jogTouch", p.jogTouch);
			settings->setValue("runTouch", p.runTouch);
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
		profile->hmdThreshold_y = hmdThreshold_y;
		profile->hmdThreshold_xz = hmdThreshold_xz;
		profile->handJogThreshold = handJogThreshold;
		profile->handRunThreshold = handRunThreshold;
		profile->useAccuracyButton = useAccuracyButton;
		profile->hmdPitchDown = hmdPitchDown;
		profile->hmdPitchUp = hmdPitchUp;
		profile->walkTouch = walkTouch;
		profile->jogTouch = jogTouch;
		profile->runTouch = runTouch;
		saveWalkInPlaceProfiles();
		OverlayController::appSettings()->sync();
		emit walkInPlaceProfilesChanged();
	}

	void WalkInPlaceTabController::applyWalkInPlaceProfile(unsigned index) {
		if (index < walkInPlaceProfiles.size()) {
			auto& profile = walkInPlaceProfiles[index];
			gameType = profile.gameType;
			controlSelect = profile.controlSelect;
			hmdThreshold_y = profile.hmdThreshold_y;
			hmdThreshold_xz = profile.hmdThreshold_xz;
			handJogThreshold = profile.handJogThreshold;
			handRunThreshold = profile.handRunThreshold;
			useAccuracyButton = profile.useAccuracyButton;
			hmdPitchDown = profile.hmdPitchDown;
			hmdPitchUp = profile.hmdPitchUp;
			walkTouch = profile.walkTouch;
			jogTouch = profile.jogTouch;
			runTouch = profile.runTouch;

			enableStepDetection(profile.stepDetectionEnabled);
			setGameStepType(profile.gameType);
			setControlSelect(profile.controlSelect);
			setHMDThreshold(profile.hmdThreshold_xz, hmdThreshold_y);
			setHandJogThreshold(profile.handJogThreshold);
			setHandRunThreshold(profile.handRunThreshold);
			setAccuracyButton(profile.useAccuracyButton);
			setHMDPitchDown(profile.hmdPitchDown);
			setHMDPitchUp(profile.hmdPitchUp);
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
		try {
			vrwalkinplace.enableStepDetection(enable);
			//for (int index = 0; index < deviceInfos.size(); index++) {
			//	deviceInfos[index]->stepDetectionEnabled = enable;
			//}
			stepDetectEnabled = enable;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting step detection: " << e.what();
		}
	}

	void WalkInPlaceTabController::setStepIntSec(float value) {
		try {
			vrwalkinplace.setStepIntSec(value);
			stepIntSec = value;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting step integration second: " << e.what();
		}
	}

	void WalkInPlaceTabController::setWalkTouch(float value) {
		try {
			vrwalkinplace.setWalkTouch(value);
			walkTouch = value;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting walk touch: " << e.what();
		}
	}

	void WalkInPlaceTabController::setJogTouch(float value) {
		try {
			vrwalkinplace.setJogTouch(value);
			jogTouch = value;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting jog touch: " << e.what();
		}
	}

	void WalkInPlaceTabController::setRunTouch(float value) {
		try {
			vrwalkinplace.setRunTouch(value);
			runTouch = value;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting run touch: " << e.what();
		}
	}

	void WalkInPlaceTabController::setHMDThreshold(float xz, float y) {
		try {
			vr::HmdVector3d_t value = { xz, y, xz };
			vrwalkinplace.setHMDThreshold(value);
			hmdThreshold_y = y;
			hmdThreshold_xz = xz;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hmd threshold: " << e.what();
		}
	}

	void WalkInPlaceTabController::setHMDPitchDown(int value) {
		try {
			vrwalkinplace.setHMDPitchDown(value);
			hmdPitchDown = value;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hmd pitch: " << e.what();
		}
	}

	void WalkInPlaceTabController::setHMDPitchUp(int value) {
		try {
			vrwalkinplace.setHMDPitchUp(value);
			hmdPitchUp = value;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hmd pitch: " << e.what();
		}
	}

	void WalkInPlaceTabController::setAccuracyButton(int buttonId) {
		try {
			vrwalkinplace.setAccuracyButton(buttonId);
			useAccuracyButton = buttonId;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hmd threshold: " << e.what();
		}
	}

	void WalkInPlaceTabController::setHandWalkThreshold(float walkThreshold) {
		try {
			handWalkThreshold = walkThreshold;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hand threshold: " << e.what();
		}
	}

	void WalkInPlaceTabController::setHandJogThreshold(float jogThreshold) {
		try {
			vrwalkinplace.setHandJogThreshold(jogThreshold);
			handJogThreshold = jogThreshold;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hand threshold: " << e.what();
		}
	}

	void WalkInPlaceTabController::setHandRunThreshold(float runThreshold) {
		try {
			vrwalkinplace.setHandRunThreshold(runThreshold);
			handRunThreshold = runThreshold;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hand threshold: " << e.what();
		}
	}

	void WalkInPlaceTabController::setGameStepType(int type) {
		try {
			vrwalkinplace.setGameStepType(type);
			gameType = type;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hand threshold: " << e.what();
		}
	}

	void WalkInPlaceTabController::setControlSelect(int control) {
		try {
			vrwalkinplace.setControlSelect(control);
			controlSelect = control;
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hand threshold: " << e.what();
		}
	}

} // namespace walkinplace
