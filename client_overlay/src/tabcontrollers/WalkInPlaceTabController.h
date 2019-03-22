
#pragma once

#include <QObject>
#include <memory>
#include <openvr.h>
#include <vector>
#include <vrwalkinplace.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <mlpack/core.hpp>

class QQuickWindow;

namespace fs = boost::filesystem;

// application namespace
namespace walkinplace {

	// forward declaration
	class OverlayController;

	struct greater
	{
		template<class T>
		bool operator()(T const &a, T const &b) const { return a > b; }
	};

	struct WalkInPlaceProfile {
		std::string profileName;
		std::string modelFile = "temp.bin";

		bool wipEnabled = false;
		bool useTrackers = false;
		bool disableHMD = false;
		bool useContDirForStraf = false;
		bool useContDirForRev = false;
		int gameType = 0;
		int hmdType = 0;
		int buttonEnables = false;
		int buttonControlSelect = 0;
		int disableButton = 0;
		bool buttonAsToggle = false;
		float minTouch = 0.35;
		float midTouch = 1.0;
		float maxTouch = 1.0;
		double minInputTime = 0.25;
	};


	struct DeviceInfo {
		std::string serial;
		vr::ETrackedDeviceClass deviceClass = vr::TrackedDeviceClass_Invalid;
		uint32_t openvrId = 0;
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

		arma::mat dataModel;
		arma::mat hmdSample;
		arma::mat cntrlSample;
		arma::mat trkrSample;

		std::string currentProfileName = "";

		std::string model_file_type = ".csv";
		std::string model_file_name = "temp";

		vr::HmdVector3d_t lastHmdPos = { 0, 0, 0 };
		vr::HmdVector3d_t lastHmdRot = { 0, 0, 0 };
		vr::HmdVector3d_t lastHmdVel = { 0, 0, 0 };
		vr::HmdVector3d_t lastTracker1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t lastTracker1Rot = { 0, 0, 0 };
		vr::HmdVector3d_t lastTracker2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t lastTracker2Rot = { 0, 0, 0 };
		vr::HmdVector3d_t lastCont1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t lastCont1Rot = { 0, 0, 0 };
		vr::HmdVector3d_t lastCont2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t lastCont2Rot = { 0, 0, 0 };

		vr::VROverlayHandle_t overlayHandle;

		bool wipEnabled = false;
		bool validSample = false;
		bool initializedDriver = false;
		bool initializedDataModel = false;
		bool initializedModelSpace = false;
		bool dataTrainingRequired = false;
		bool identifyControlTimerSet = true;

		bool buttonAsToggle = false;
		bool buttonEnables = false;
		bool holdingButton = false;
		bool useTrackers = false;
		bool disableHMD = false;
		bool validTRKRSample = false;
		bool useContDirForStraf = false;
		bool useContDirForRev = false;
		bool pressedFlag = false;

		int gameType = 0;
		int hmdType = 0;
		int buttonControlSelect = 0;
		int controlSelectOverlayHandle = -1;
		int vive_controller_model_index = -1;
		int disableButton = -1;
		int unnTouchedCount = 0;
		int lastValidHMDSampleMKi = 0;
		int lastValidCNTRLSampleMKi = 0;
		int lastValidTRKRSampleMKi = 0;

		uint64_t controller1ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t controller2ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t controlUsedID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t hmdID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t tracker1ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t tracker2ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t vrLocoContID = vr::k_unTrackedDeviceIndexInvalid;

		float haltHMDVariance = 0.09;
		float beginHMDVariance = 0.05;
		float haltHMDRotVariance = 0.09;
		float beginHMDRotVariance = 0.04;
		float haltTRKRVariance = 0.09;
		float beginTRKRVariance = 0.05;
		float sNValidTouch = 0;
		float minTouch = 0.35;
		float midTouch = 0.9;
		float maxTouch = 1.0;
		float touchX = 0;
		float touchY = 0;
		float hmdYaw = 0;
		float contYaw = 0;
		float contRoll = 0;
		float contPitch = 0;

		double timeLastTick = 0.0;
		double timeLastCNTRLSN = 0.0;
		double identifyControlLastTime = 99999;
		double identifyControlTimeOut = 6000;
		double timeLastGraphPoint = 0.0;

		std::pair<float, int> computeSNDelta(arma::mat sN, arma::mat mN);
		arma::mat computeKVDelta(arma::mat sN, arma::mat mN, int kV);

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
		Q_INVOKABLE double getStepTime();
		Q_INVOKABLE int getGameType();
		Q_INVOKABLE int getHMDType();
		Q_INVOKABLE int getButtonControlSelect();
		Q_INVOKABLE int getDisableButton();
		Q_INVOKABLE bool getUseTrackers();
		Q_INVOKABLE bool getDisableHMD();
		Q_INVOKABLE bool getUseContDirForStraf();
		Q_INVOKABLE bool getUseContDirForRev();
		Q_INVOKABLE float getWalkTouch();
		Q_INVOKABLE float getJogTouch();
		Q_INVOKABLE float getRunTouch();
		Q_INVOKABLE bool getUseButtonAsToggle();
		Q_INVOKABLE bool getButtonEnables();
		Q_INVOKABLE bool getDeviceEnabled(int devClass, int devIdx, int mode);
		Q_INVOKABLE bool isWIPEnabled();
		Q_INVOKABLE bool isStepDetected();
		Q_INVOKABLE QList<qreal> getGraphPoses();
		Q_INVOKABLE QList<qreal> getValidHMDSample();
		Q_INVOKABLE QList<qreal> getValidCNTRLSample(); 
		Q_INVOKABLE QList<qreal> getValidTRKRSample();
		Q_INVOKABLE void applyVirtualStep();
		Q_INVOKABLE QList<qreal> getModelData();
		Q_INVOKABLE QList<qreal> trainingDataSample(float scaleSpeed, double tdiff);
		Q_INVOKABLE void completeTraining();

		Q_INVOKABLE unsigned getProfileCount();
		Q_INVOKABLE QString getProfileName(unsigned index);

		Q_INVOKABLE QList<QString> getDataModelNames();

	public slots:
		void enableWIP(bool enable);
		void setMinInputTime(double value);
		void setDisableHMD(bool val);
		void setDirectionDevice(int choice);
		void enableDevice(int deviceClass, int devIdx, bool enable, int mode);
		void setDisableButton(int buttonId);
		void setButtonAsToggle(bool val);
		void setButtonEnables(bool val);
		void setMinTouch(float value);
		void setMidTouch(float value);
		void setMaxTouch(float value);
		void setUseContDirForStraf(bool val);
		void setUseContDirForRev(bool val);
		void setGameStepType(int gameType);
		void setHMDType(int gameType);
		void setButtonControlSelect(int control);
		void setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex, float r, float g, float b, float sx, float sy, float sz);
		void applyStepPoseDetect();

		bool buttonStatus();

		void stopMovement(uint32_t deviceId);
		void stopClickMovement(uint32_t deviceId);
		void applyAxisMovement(uint32_t deviceId, vr::VRControllerAxis_t axisState);
		void applyClickMovement(uint32_t deviceId);
		void applyGripMovement(uint32_t deviceId);

		void updateButtonState(uint32_t deviceId, bool firstController);

		void addProfile(QString name);
		void applyProfile(unsigned index);
		void deleteProfile(unsigned index);

		void reloadSettings();
		void reloadProfiles();
		void saveSettings();
		void saveProfiles();

		void addDataModel(QString name);
		void applyDataModel(QString name);
		void deleteDataModel(QString name);

	};

} // namespace walkinplace
