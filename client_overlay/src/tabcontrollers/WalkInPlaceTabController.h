
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
	bool useButtonAsToggle = false;
	bool useTrackers = false;
	bool disableHMD = false;
	bool scaleTouchWithSwing = false;
	bool useContDirForStraf = false;
	bool useContDirForRev = false;
	int gameType = 0;
	int hmdType = 0;
	int controlSelect = 0;
	int flipButtonUse = false;
	int buttonControlSelect = 0;
	int useAccuracyButton = 0;
	int hmdPitchDown = 35;
	int hmdPitchUp = 25;
	float handWalkThreshold = 0.02;
	float handJogThreshold = 1.1;
	float handRunThreshold = 2.0;
	float walkTouch = 0.35;
	float jogTouch = 1.0;
	float runTouch = 1.0;
	float hmdThreshold_y = 0.12;
	float hmdThreshold_xz = 0.27;
	float trackerThreshold_xz = 0.27;
	float trackerThreshold_y = 0.10;
	double stepTime = 0.5;
};


struct DeviceInfo {
	std::string serial;
	vr::ETrackedDeviceClass deviceClass = vr::TrackedDeviceClass_Invalid;
	uint32_t openvrId = 0;
	int deviceStatus = 0; // 0 .. Normal, 1 .. Disconnected/Suspended
	int deviceMode = 0; // 0  normal, 1 step detection
	bool stepDetectionEnabled;
	uint32_t renderModelIndex = 0;
	vr::VROverlayHandle_t renderModelOverlay = vr::k_ulOverlayHandleInvalid;
	std::string renderModelOverlayName;
};


class WalkInPlaceTabController : public QObject {
	Q_OBJECT

private:
	OverlayController * parent;
	QQuickWindow* widget;
	vrwalkinplace::VRWalkInPlace vrwalkinplace;

	std::vector<std::shared_ptr<DeviceInfo>> deviceInfos;
	uint32_t maxValidDeviceId = 0;

	std::thread identifyThread;

	unsigned settingsUpdateCounter = 0;

	std::vector<WalkInPlaceProfile> walkInPlaceProfiles;

	vr::TrackedDevicePose_t latestDevicePoses[vr::k_unMaxTrackedDeviceCount];
	vr::HmdVector3d_t hmdVel = { 0, 0, 0 };
	vr::HmdVector3d_t lastHmdPos = { 0, 0, 0 };
	vr::HmdVector3d_t tracker1Vel = { 0, 0, 0 };
	vr::HmdVector3d_t tracker2Vel = { 0, 0, 0 };
	vr::HmdVector3d_t cont1Vel = { 0, 0, 0 };
	vr::HmdVector3d_t cont2Vel = { 0, 0, 0 };
	vr::HmdVector3d_t _hmdThreshold = { 0.27, 0.12, 0.27 };
	vr::HmdVector3d_t _trackerThreshold = { 0.27, 0.10, 0.27 };
	vr::HmdVector3d_t hmdForward = { 0,0,-1 };
	vr::VROverlayHandle_t overlayHandle;

	bool identifyControlTimerSet = false;
	bool stepDetectEnabled = false;
	bool _stepPoseDetected = false;
	bool betaEnabled = false;
	bool useButtonAsToggle = false;
	bool flipButtonUse = false;
	bool useTrackers = false;
	bool disableHMD = false;
	bool trackerStepDetected = false;
	bool scaleSpeedWithSwing = false;
	bool useContDirForStraf = false;
	bool useContDirForRev = false;
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
	bool g_accuracyButtonWithTouch = false;
	bool showingStepGraph = false;
	bool showGraphDetectTO = true;
	int gameType = 0;
	int hmdType = 0;
	int controlSelect = 0;
	int buttonControlSelect = 0;
	int controlSelectOverlayHandle = -1;
	int vive_controller_model_index = -1;
	int useAccuracyButton = 2;
	int g_AccuracyButton = -1;
	int _teleportUnpressed = true;
	int _hasUnTouchedStepAxis = 50;
	int peaksCount = 0;
	int _controllerDeviceIds[2] = { -1, -1 };
	int _controlUsedID = -1;
	int stepPeaksToStart = 3;
	int contSampleCount = 0;
	float hmdYaw = 0;
	float contYaw = 0;
	float contRoll = 0;
	float contPitch = 0;
	float touchX = 0;
	float touchY = 0;
	float handWalkThreshold = 0.02;
	float handJogThreshold = 1.0;
	float handRunThreshold = 2.0;
	float walkTouch = 0.35;
	float jogTouch = 1.0;
	float runTouch = 1.0;
	float minTouch = 0.45;
	float trackerLastYVel = 0;
	float hmdLastYVel = 0;
	float cont1LastYVel = 0;
	float cont2LastYVel = 0;
	float stepPeaksFullSpeed = 13.0;
	float avgContYVel = 0.0;
	double _stepFrequencyMin = 250;
	double _stepIntegrateSteps = 0.0;
	double _jogIntegrateSteps = 0.0;
	double _runIntegrateSteps = 0.0;
	double _stepIntegrateStepLimit = 500;
	double _timeLastTick = 0.0;
	double _velStepTime = 0.0;
	double _timeLastStepPeak = 0.0;
	double _timeLastTrackerStep = 0.0;
	double _timeLastNod = 0.0;
	double identifyControlLastTime = 99999;
	double identifyControlTimeOut = 6000;
	double _timeLastGraphPoint = 0.0;

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
	Q_INVOKABLE int getHMDType();
	Q_INVOKABLE int getControlSelect();
	Q_INVOKABLE int getAccuracyButtonControlSelect();
	Q_INVOKABLE int getAccuracyButton();
	Q_INVOKABLE float getHMDXZThreshold();
	Q_INVOKABLE float getHMDYThreshold();
	Q_INVOKABLE bool getUseTrackers();
	Q_INVOKABLE bool getDisableHMD();
	Q_INVOKABLE float getTrackerXZThreshold();
	Q_INVOKABLE float getTrackerYThreshold();
	Q_INVOKABLE float getHandWalkThreshold();
	Q_INVOKABLE float getHandJogThreshold();
	Q_INVOKABLE float getHandRunThreshold();
	Q_INVOKABLE bool getUseContDirForStraf();
	Q_INVOKABLE bool getUseContDirForRev();
	Q_INVOKABLE bool getScaleTouchWithSwing();
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
	void setDisableHMD(bool val);
	void setTrackerThreshold(float xz, float y);
	void setAccuracyButton(int buttonId);
	void setAccuracyButtonAsToggle(bool val);
	void disableByButton(bool val);
	void setHandWalkThreshold(float walkThreshold);
	void setHandJogThreshold(float jogThreshold);
	void setHandRunThreshold(float runThreshold);
	void setScaleTouchWithSwing(bool val);
	void setWalkTouch(float value);
	void setJogTouch(float value);
	void setRunTouch(float value);
	void setUseContDirForStraf(bool val);
	void setUseContDirForRev(bool val);
	void setGameStepType(int gameType);
	void setHMDType(int gameType);
	void setControlSelect(int control);
	void setAccuracyButtonControlSelect(int control);
	void setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex, float r, float g, float b, float sx, float sy, float sz);
	void applyStepPoseDetect();

	bool accuracyButtonOnOrDisabled();
	bool upAndDownStepCheck(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold, double roll, double pitch);
	bool nodCheck(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold);
	bool sideToSideStepCheck(vr::HmdVector3d_t vel, vr::HmdVector3d_t threshold);
	bool isJoggingStep(float * vel);
	bool isRunningStep(float * vel);
	float getScaledTouch(float minTouch, float maxTouch, float avgVel, float maxVel);

	void stopMovement(uint32_t deviceId);
	void stopClickMovement(uint32_t deviceId);
	void applyAxisMovement(uint32_t deviceId, vr::VRControllerAxis_t axisState);
	void applyClickMovement(uint32_t deviceId);
	void applyGripMovement(uint32_t deviceId);
	void axisEvent(int deviceId, int axisId, float x, float y);
	void buttonEvent(int deviceId, int buttonId, int buttonState);

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
