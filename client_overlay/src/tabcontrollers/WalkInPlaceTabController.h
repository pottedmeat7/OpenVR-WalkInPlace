
#pragma once

#include <QObject>
#include <memory>
#include <openvr.h>
#include <vrwalkinplace.h>

class QQuickWindow;
// application namespace
namespace walkinplace {

// forward declaration
class OverlayController;


struct WalkInPlaceProfile {
	std::string profileName;

	bool stepDetectionEnabled = false;
	int gameType = 1;
	int controlSelect = 2;
	int hmdPitchDown = 20;
	int hmdPitchUp = 15;
	float handWalkThreshold = 0.02;
	float handJogThreshold = 0.50;
	float handRunThreshold = 1.70;
	float walkTouch = 0.6;
	float jogTouch = 0.77;
	float runTouch = 1.0;
	float hmdThreshold_y = 0.13;
	float hmdThreshold_xz = 0.27;
	int useAccuracyButton = 0;
};


struct DeviceInfo {
	std::string serial;
	vr::ETrackedDeviceClass deviceClass = vr::TrackedDeviceClass_Invalid;
	uint32_t openvrId = 0;
	int deviceStatus = 0; // 0 .. Normal, 1 .. Disconnected/Suspended
	int deviceMode = 0; // 0  normal, 1 step detection
	bool stepDetectionEnabled;
};


class WalkInPlaceTabController : public QObject {
	Q_OBJECT

private:
	OverlayController* parent;
	QQuickWindow* widget;
	vrwalkinplace::VRWalkInPlace vrwalkinplace;

	std::vector<std::shared_ptr<DeviceInfo>> deviceInfos;
	uint32_t maxValidDeviceId = 0;

	std::vector<WalkInPlaceProfile> walkInPlaceProfiles;

	unsigned settingsUpdateCounter = 0;

	std::thread identifyThread;

	int gameType = 1;
	int controlSelect = 2;
	bool stepDetectEnabled = false;
	float hmdThreshold_y = 0.13;
	float hmdThreshold_xz = 0.27;
	int useAccuracyButton = 5;
	int hmdPitchDown = 20;
	int hmdPitchUp = 15;
	float stepIntSec = 0.07;
	float handWalkThreshold = 0.02;
	float handJogThreshold = 0.50;
	float handRunThreshold = 1.70;
	float walkTouch = 0.6;
	float jogTouch = 0.77;
	float runTouch = 1.0;
	vr::VROverlayHandle_t overlayHandle;
	

public:
	~WalkInPlaceTabController();
	void initStage1();
	void initStage2(OverlayController* parent, QQuickWindow* widget);

	void eventLoopTick(vr::TrackedDevicePose_t* devicePoses);
	void handleEvent(const vr::VREvent_t& vrEvent);

	Q_INVOKABLE unsigned getDeviceCount();
	Q_INVOKABLE QString getDeviceSerial(unsigned index);
	Q_INVOKABLE unsigned getDeviceId(unsigned index);
	Q_INVOKABLE int getDeviceClass(unsigned index);
	Q_INVOKABLE int getDeviceState(unsigned index);
	Q_INVOKABLE int getDeviceMode(unsigned index);
	Q_INVOKABLE float getStepIntSec();
	Q_INVOKABLE int getGameType();
	Q_INVOKABLE int getControlSelect();
	Q_INVOKABLE int getAccuracyButton();
	Q_INVOKABLE int getHMDPitchDown();
	Q_INVOKABLE int getHMDPitchUp();
	Q_INVOKABLE float getHMDXZThreshold();
	Q_INVOKABLE float getHMDYThreshold();
	Q_INVOKABLE float getHandWalkThreshold();
	Q_INVOKABLE float getHandJogThreshold();
	Q_INVOKABLE float getHandRunThreshold();
	Q_INVOKABLE float getWalkTouch();
	Q_INVOKABLE float getJogTouch();
	Q_INVOKABLE float getRunTouch();
	Q_INVOKABLE bool isStepDetectionEnabled();

	void reloadWalkInPlaceSettings();
	void reloadWalkInPlaceProfiles();
	void saveWalkInPlaceSettings();
	void saveWalkInPlaceProfiles();

	Q_INVOKABLE unsigned getWalkInPlaceProfileCount();
	Q_INVOKABLE QString getWalkInPlaceProfileName(unsigned index);

public slots:
    void enableStepDetection(bool enable);
	void setStepIntSec(float value);
	void setHMDThreshold(float xz, float y);
	void setAccuracyButton(int buttonId);
	void setHMDPitchDown(int value);
	void setHMDPitchUp(int value);
	void setHandWalkThreshold(float walkThreshold);
	void setHandJogThreshold(float jogThreshold);
	void setHandRunThreshold(float runThreshold);
	void setWalkTouch(float value);
	void setJogTouch(float value);
	void setRunTouch(float value);
	void setGameStepType(int gameType);
	void setControlSelect(int control);

	void addWalkInPlaceProfile(QString name);
	void applyWalkInPlaceProfile(unsigned index);
	void deleteWalkInPlaceProfile(unsigned index);


signals:
	void deviceCountChanged(unsigned deviceCount);
	void deviceInfoChanged(unsigned index);
	void walkInPlaceProfilesChanged();
};

} // namespace walkinplace
