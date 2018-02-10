
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
	bool useButtonAsToggle = false;
	bool useTrackers = false;
	int flipButtonUse = false;
	int hmdPitchDown = 35;
	int hmdPitchUp = 25;
	double stepTime = 0.7;
	float handWalkThreshold = 0.02;
	float handJogThreshold = 0.50;
	float handRunThreshold = 1.70;
	float walkTouch = 0.6;
	float jogTouch = 0.77;
	float runTouch = 1.0;
	float hmdThreshold_y = 0.27;
	float hmdThreshold_xz = 0.27;
	float tracker_xz = 0.27;
	float tracker_y = 0.27;
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

	vr::TrackedDevicePose_t latestDevicePoses[vr::k_unMaxTrackedDeviceCount];
	vr::HmdVector2_t hmdVel;
	vr::HmdVector2_t trackerVel;
	vr::HmdVector2_t cont1Vel;
	vr::HmdVector2_t cont2Vel;

	double _timeLastGraphPoint = 0.0;

	std::vector<WalkInPlaceProfile> walkInPlaceProfiles;

	unsigned settingsUpdateCounter = 0;

	std::thread identifyThread;

	bool showStepGraph = false;
	int gameType = 1;
	int controlSelect = 2;
	bool stepDetectEnabled = false;
	bool betaEnabled = false;
	vr::HmdVector3d_t _hmdThreshold = { 0.27, 0.27, 0.27 };
	vr::HmdVector3d_t _trackerThreshold = { 0.27, 0.27, 0.27 };
	int useAccuracyButton = 5;
	bool useButtonAsToggle = false;
	bool flipButtonUse = false;
	bool useTrackers = false;
	int hmdPitchDown = 35;
	int hmdPitchUp = 25;
	float handWalkThreshold = 0.02;
	float handJogThreshold = 0.50;
	float handRunThreshold = 1.70;
	float walkTouch = 0.6;
	float jogTouch = 0.77;
	float runTouch = 1.0;
	float minTouch = 0.45;
	vr::VROverlayHandle_t overlayHandle;
	int showingStepGraph = 101;

	int _teleportUnpressed = true;
	int _hasUnTouchedStepAxis = 50;
	
	int peaksCount = 0;
	float trackerLastYVel = 0;
	float hmdLastYVel = 0;
	float cont1LastYVel = 0;
	float cont2LastYVel = 0;
	int _controllerDeviceIds[2];
	int _controlSelect = 1;
	int _controlUsedID = -1;
	double _timeLastTick = 0.0;
	double _timeLastStepPeak = 0.0;
	int stepPeaksToStart = 3;
	float stepPeaksFullSpeed = 13.0;
	double _stepFrequencyMin = 250;
	bool _stepPoseDetected = false;
	double _stepIntegrateSteps = 0.0;
	double _jogIntegrateSteps = 0.0;
	double _runIntegrateSteps = 0.0;
	double _stepIntegrateStepLimit = 0.7 * 1000;

	bool g_stepDetectEnabled = false;
	bool g_disableGameLocomotion = false;
	bool g_isHoldingAccuracyButton = false;
	bool g_isHoldingAccuracyButton1 = false;
	bool g_isHoldingAccuracyButton2 = false;
	bool g_useButtonAsToggle = false;
	bool g_buttonToggled = true;
	bool g_stepPoseDetected = false;
	bool g_jogPoseDetected = false;
	bool g_runPoseDetected = false;
	int g_AccuracyButton = -1;
	bool g_accuracyButtonWithTouch = false;
	

public:
	~WalkInPlaceTabController();
	void initStage1();
	void initStage2(OverlayController* parent, QQuickWindow* widget);

	void eventLoopTick();
	void handleEvent(const vr::VREvent_t& vrEvent);

	Q_INVOKABLE unsigned getDeviceCount();
	Q_INVOKABLE QString getDeviceSerial(unsigned index);
	Q_INVOKABLE unsigned getDeviceId(unsigned index);
	Q_INVOKABLE int getDeviceClass(unsigned index);
	Q_INVOKABLE int getDeviceState(unsigned index);
	Q_INVOKABLE int getDeviceMode(unsigned index);
	Q_INVOKABLE double getStepTime();
	Q_INVOKABLE int getGameType();
	Q_INVOKABLE int getControlSelect();
	Q_INVOKABLE int getAccuracyButton();
	Q_INVOKABLE int getHMDPitchDown();
	Q_INVOKABLE int getHMDPitchUp();
	Q_INVOKABLE float getHMDXZThreshold();
	Q_INVOKABLE float getHMDYThreshold();
	Q_INVOKABLE bool getUseTrackers();
	Q_INVOKABLE float getTrackerXZThreshold();
	Q_INVOKABLE float getTrackerYThreshold();
	Q_INVOKABLE float getHandWalkThreshold();
	Q_INVOKABLE float getHandJogThreshold();
	Q_INVOKABLE float getHandRunThreshold();
	Q_INVOKABLE float getWalkTouch();
	Q_INVOKABLE float getJogTouch();
	Q_INVOKABLE float getRunTouch();
	Q_INVOKABLE bool getAccuracyButtonIsToggle();
	Q_INVOKABLE bool getAccuracyButtonFlip();
	Q_INVOKABLE bool isStepDetectionEnabled();
	Q_INVOKABLE bool isStepDetected();
	Q_INVOKABLE QList<qreal> getGraphPoses();
	Q_INVOKABLE void setupStepGraph();

	void reloadWalkInPlaceSettings();
	void reloadWalkInPlaceProfiles();
	void saveWalkInPlaceSettings();
	void saveWalkInPlaceProfiles();

	Q_INVOKABLE unsigned getWalkInPlaceProfileCount();
	Q_INVOKABLE QString getWalkInPlaceProfileName(unsigned index);

public slots:
    void enableStepDetection(bool enable);
	void enableBeta(bool enable);
	void setStepTime(double value);
	void setHMDThreshold(float xz, float y);
	void setUseTrackers(bool val);
	void setTrackerThreshold(float xz, float y);
	void setAccuracyButton(int buttonId);
	void setAccuracyButtonAsToggle(bool val);
	void setAccuracyButtonFlip(bool val);
	void setHandWalkThreshold(float walkThreshold);
	void setHandJogThreshold(float jogThreshold);
	void setHandRunThreshold(float runThreshold);
	void setWalkTouch(float value);
	void setJogTouch(float value);
	void setRunTouch(float value);
	void setGameStepType(int gameType);
	void setControlSelect(int control);
	void applyStepPoseDetect();

	bool isTakingStep(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold, double roll, double pitch);
	bool isShakingHead(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold);
	bool isStepingInPlace(float * pos);
	bool isJoggingStep(float * vel);
	bool isRunningStep(float * vel);

	void updateAccuracyButtonState(uint32_t deviceId, bool firstController);

	void addWalkInPlaceProfile(QString name);
	void applyWalkInPlaceProfile(unsigned index);
	void deleteWalkInPlaceProfile(unsigned index);


signals:
	void deviceCountChanged(unsigned deviceCount);
	void deviceInfoChanged(unsigned index);
	void walkInPlaceProfilesChanged();
};

} // namespace walkinplace
