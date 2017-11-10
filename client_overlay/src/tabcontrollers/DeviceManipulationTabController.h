
#pragma once

#include <QObject>
#include <memory>
#include <openvr.h>
#include <vrwalkinplace.h>

class QQuickWindow;
// application namespace
namespace inputemulator {

// forward declaration
class OverlayController;


struct DeviceManipulationProfile {
	std::string profileName;

	bool includesDeviceOffsets = false;
	bool deviceOffsetsEnabled = false;
	vr::HmdVector3d_t worldFromDriverTranslationOffset;
	vr::HmdVector3d_t worldFromDriverRotationOffset;
	vr::HmdVector3d_t driverFromHeadTranslationOffset;
	vr::HmdVector3d_t driverFromHeadRotationOffset;
	vr::HmdVector3d_t driverTranslationOffset;
	vr::HmdVector3d_t driverRotationOffset;
};


struct DeviceInfo {
	std::string serial;
	vr::ETrackedDeviceClass deviceClass = vr::TrackedDeviceClass_Invalid;
	uint32_t openvrId = 0;
	int deviceStatus = 0; // 0 .. Normal, 1 .. Disconnected/Suspended
	int deviceMode = 0; // 0 .. Default, 1 .. Fake Disconnected, 2 .. Redirect Source, 3 .. Redirect Target, 4 .. Motion Compensation .. 5 step detection
	bool deviceOffsetsEnabled;
	bool stepDetectionEnabled;
	vr::HmdVector3d_t worldFromDriverRotationOffset;
	vr::HmdVector3d_t worldFromDriverTranslationOffset;
	vr::HmdVector3d_t driverFromHeadRotationOffset;
	vr::HmdVector3d_t driverFromHeadTranslationOffset;
	vr::HmdVector3d_t deviceRotationOffset;
	vr::HmdVector3d_t deviceTranslationOffset;
	uint32_t renderModelIndex = 0;
	vr::VROverlayHandle_t renderModelOverlay = vr::k_ulOverlayHandleInvalid;
	std::string renderModelOverlayName;
};


class DeviceManipulationTabController : public QObject {
	Q_OBJECT

private:
	OverlayController* parent;
	QQuickWindow* widget;
	vrwalkinplace::VRWalkInPlace vrInputEmulator;

	std::vector<std::shared_ptr<DeviceInfo>> deviceInfos;
	uint32_t maxValidDeviceId = 0;

	std::vector<DeviceManipulationProfile> deviceManipulationProfiles;

	uint32_t motionCompensationVelAccMode = 0;

	unsigned settingsUpdateCounter = 0;

	std::thread identifyThread;

	float stepIntSec = 0.07;

	vr::HmdVector3d_t hmdThreshold = { 0.13, 0.13, 0.13 };
	float handJogThreshold = 0.40;
	float handRunThreshold = 1.70;

public:
	~DeviceManipulationTabController();
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
	Q_INVOKABLE bool deviceOffsetsEnabled(unsigned index);
	Q_INVOKABLE float getStepIntSec();
	Q_INVOKABLE float getHMDXZThreshold();
	Q_INVOKABLE float getHMDYThreshold();
	Q_INVOKABLE float getHandJogThreshold();
	Q_INVOKABLE float getHandRunThreshold();
	Q_INVOKABLE bool isStepDetectionEnabled();
	Q_INVOKABLE double getWorldFromDriverRotationOffset(unsigned index, unsigned axis);
	Q_INVOKABLE double getWorldFromDriverTranslationOffset(unsigned index, unsigned axis);
	Q_INVOKABLE double getDriverFromHeadRotationOffset(unsigned index, unsigned axis);
	Q_INVOKABLE double getDriverFromHeadTranslationOffset(unsigned index, unsigned axis);
	Q_INVOKABLE double getDriverRotationOffset(unsigned index, unsigned axis);
	Q_INVOKABLE double getDriverTranslationOffset(unsigned index, unsigned axis);
	Q_INVOKABLE unsigned getMotionCompensationVelAccMode();

	void reloadDeviceManipulationSettings();
	void reloadDeviceManipulationProfiles();
	void saveDeviceManipulationSettings();
	void saveDeviceManipulationProfiles();

	Q_INVOKABLE unsigned getDeviceManipulationProfileCount();
	Q_INVOKABLE QString getDeviceManipulationProfileName(unsigned index);

	Q_INVOKABLE unsigned getRenderModelCount();
	Q_INVOKABLE QString getRenderModelName(unsigned index);
	Q_INVOKABLE bool updateDeviceInfo(unsigned index);



public slots:
    void enableStepDetection(bool enable);
	void setStepIntSec(float value);
	void setHMDThreshold(float x, float y, float z);
	void setHandJogThreshold(float jogThreshold);
	void setHandRunThreshold(float runThreshold);
	void setGameStepType(int gameType);
	void setWorldFromDriverRotationOffset(unsigned index, double x, double y, double z, bool notify = true);
	void setWorldFromDriverTranslationOffset(unsigned index, double yaw, double pitch, double roll, bool notify = true);
	void setDriverFromHeadRotationOffset(unsigned index, double x, double y, double z, bool notify = true);
	void setDriverFromHeadTranslationOffset(unsigned index, double yaw, double pitch, double roll, bool notify = true);
	void setDriverRotationOffset(unsigned index, double x, double y, double z, bool notify = true);
	void setDriverTranslationOffset(unsigned index, double yaw, double pitch, double roll, bool notify = true);
	void setDeviceMode(unsigned index, unsigned mode, unsigned targedIndex, bool notify = true);
	void triggerHapticPulse(unsigned index);
	void setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex);

	void addDeviceManipulationProfile(QString name, unsigned deviceIndex, bool includesDeviceOffsets, bool includesMotionCompensationSettings);
	void applyDeviceManipulationProfile(unsigned index, unsigned deviceIndex);
	void deleteDeviceManipulationProfile(unsigned index);

	void setMotionCompensationVelAccMode(unsigned mode, bool notify = true);

signals:
	void deviceCountChanged(unsigned deviceCount);
	void deviceInfoChanged(unsigned index);
	void motionCompensationSettingsChanged();
	void deviceManipulationProfilesChanged();
	void motionCompensationVelAccModeChanged(unsigned mode);
};

} // namespace inputemulator
