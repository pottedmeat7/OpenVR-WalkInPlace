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
		//reloadWalkInPlaceProfiles();
		//reloadWalkInPlaceSettings();
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
				unsigned i = 0;
				for (auto info : deviceInfos) {
					bool hasDeviceInfoChanged = updateDeviceInfo(i);
					unsigned status = devicePoses[info->openvrId].bDeviceIsConnected ? 0 : 1;
					if (info->deviceMode == 0 && info->deviceStatus != status) {
						info->deviceStatus = status;
						hasDeviceInfoChanged = true;
					}
					if (hasDeviceInfoChanged) {
						emit deviceInfoChanged(i);
					}
					++i;
				}
				bool newDeviceAdded = false;
				for (uint32_t id = maxValidDeviceId + 1; id < vr::k_unMaxTrackedDeviceCount; ++id) {
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

							try {
								vrwalkinplace::DeviceInfo info2;
								vrwalkinplace.getDeviceInfo(info->openvrId, info2);
								info->deviceMode = info2.deviceMode;
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
		for (int index = 0; index < deviceInfos.size(); index++) {
			if (deviceInfos[index]->stepDetectionEnabled) {
				return true;
			}
		}
		return false;
	}

	float WalkInPlaceTabController::getStepIntSec() {
		return stepIntSec;
	}

	int WalkInPlaceTabController::getGameType() {
		return gameType;
	}

	float WalkInPlaceTabController::getHMDXZThreshold() {
		return hmdThreshold.v[0];
	}

	float WalkInPlaceTabController::getHMDYThreshold() {
		return hmdThreshold.v[1];
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


#define WALKINPLACESETTINGS_GETHMDTHRESHOLDS(name) { \
	double y = settings->value(#name ## "_y", 0.13).toDouble(); \
	double xz = settings->value(#name ## "_xz", 0.13).toDouble(); \
	entry.name = { xz, y, xz }; \
}

#define WALKINPLACESETTINGS_GETWALKSWINGVAL(name) { \
	double walk = settings->value(#name ## "_walk", 0.02).toDouble(); \
	entry.name = walk; \
}

#define WALKINPLACESETTINGS_GETJOGSWINGVAL(name) { \
	double jog = settings->value(#name ## "_jog", 0.4).toDouble(); \
	entry.name = jog; \
}

#define WALKINPLACESETTINGS_GETRUNSWINGVAL(name) { \
	double run = settings->value(#name ## "_run", 1.7).toDouble(); \
	entry.name = run; \
}

#define WALKINPLACESETTINGS_GETGAMETYPE(name) { \
	double type = settings->value(#name ## "_gametype", 1).toInt(); \
	entry.name = type; \
}

	void WalkInPlaceTabController::reloadWalkInPlaceSettings() {
		//auto settings = OverlayController::appSettings();
		//settings->beginGroup("walkInPlaceSettings");
		//settings->endGroup();
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
			WALKINPLACESETTINGS_GETHMDTHRESHOLDS(hmdThreshold);
			WALKINPLACESETTINGS_GETWALKSWINGVAL(handWalkThreshold);
			WALKINPLACESETTINGS_GETJOGSWINGVAL(handJogThreshold);
			WALKINPLACESETTINGS_GETRUNSWINGVAL(handRunThreshold);
			WALKINPLACESETTINGS_GETGAMETYPE(gameType);
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



#define WALKINPLACESETTINGS_WRITEHMDTHRESHOLDS(name) { \
	auto& vec = p.name; \
	settings->setValue(#name ## "_y", vec.v[1]); \
	settings->setValue(#name ## "_xz", vec.v[0]); \
}

#define WALKINPLACESETTINGS_WRITEWALKSWINGVAL(name) { \
    auto& walk = p.name; \
	settings->setValue(#name ## "_walk", walk); \
}

#define WALKINPLACESETTINGS_WRITEJOGSWINGVAL(name) { \
    auto& jog = p.name; \
	settings->setValue(#name ## "_jog", jog); \
}

#define WALKINPLACESETTINGS_WRITERUNSWINGVAL(name) { \
    auto& run = p.name; \
	settings->setValue(#name ## "_run", run); \
}

#define WALKINPLACESETTINGS_WRITEGAMETYPE(name) { \
    auto& type = p.name; \
	settings->setValue(#name ## "_gametype", type); \
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
			WALKINPLACESETTINGS_WRITEHMDTHRESHOLDS(hmdThreshold);
			WALKINPLACESETTINGS_WRITEWALKSWINGVAL(handWalkThreshold);
			WALKINPLACESETTINGS_WRITEJOGSWINGVAL(handJogThreshold);
			WALKINPLACESETTINGS_WRITERUNSWINGVAL(handRunThreshold);
			WALKINPLACESETTINGS_WRITEGAMETYPE(gameType);
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
		profile->hmdThreshold = hmdThreshold;
		profile->handJogThreshold = handJogThreshold;
		profile->handRunThreshold = handRunThreshold;
		profile->gameType = gameType;
		saveWalkInPlaceProfiles();
		OverlayController::appSettings()->sync();
		emit walkInPlaceProfilesChanged();
	}

	void WalkInPlaceTabController::applyWalkInPlaceProfile(unsigned index) {
		if (index < walkInPlaceProfiles.size()) {
			auto& profile = walkInPlaceProfiles[index];
			setHMDThreshold(profile.hmdThreshold.v[0], profile.hmdThreshold.v[1], profile.hmdThreshold.v[2]);
			setHandJogThreshold(profile.handJogThreshold);
			setHandRunThreshold(profile.handRunThreshold);
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
			for (int index = 0; index < deviceInfos.size(); index++) {
				deviceInfos[index]->stepDetectionEnabled = enable;
			}
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

	void WalkInPlaceTabController::setHMDThreshold(float x, float y, float z) {
		try {
			vr::HmdVector3d_t value = { x, y, z };
			vrwalkinplace.setHMDThreshold(value);
			hmdThreshold = value;
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

	void WalkInPlaceTabController::setGameStepType(int gameType) {
		try {
			vrwalkinplace.setGameStepType(gameType);
		}
		catch (const std::exception& e) {
			LOG(ERROR) << "Exception caught while setting hand threshold: " << e.what();
		}
	}

	bool WalkInPlaceTabController::updateDeviceInfo(unsigned index) {
		bool retval = false;
		if (index < deviceInfos.size()) {
			try {
				vrwalkinplace::DeviceInfo info;
				vrwalkinplace.getDeviceInfo(deviceInfos[index]->openvrId, info);
				if (deviceInfos[index]->deviceMode != info.deviceMode) {
					deviceInfos[index]->deviceMode = info.deviceMode;
					retval = true;
				}
				if (deviceInfos[index]->stepDetectionEnabled != info.stepDetectionEnabled) {
					deviceInfos[index]->stepDetectionEnabled = info.stepDetectionEnabled;
					retval = true;
				}
			}
			catch (std::exception& e) {
				LOG(ERROR) << "Exception caught while getting device info: " << e.what();
			}
		}
		return retval;
	}

} // namespace walkinplace
