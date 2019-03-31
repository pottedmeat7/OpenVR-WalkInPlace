
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
		std::string modelFile = "temp.csv";

		bool wipEnabled = false;
		bool useTrackers = false;
		bool trackHMDVel = true;
		bool trackHMDRot = true;
		int gameType = 0;
		int hmdType = 0;
		int buttonEnables = false;
		int buttonControlSelect = 0;
		int disableButton = 0;
		float minTouch = 0.35;
		float midTouch = 0.83;
		float maxTouch = 1.0;
		int cntrl1Type = vr::TrackedDeviceClass::TrackedDeviceClass_Controller;
		int cntrl2Type = vr::TrackedDeviceClass::TrackedDeviceClass_Controller;
		int trkr1Type = vr::TrackedDeviceClass::TrackedDeviceClass_GenericTracker;
		int trkr2Type = vr::TrackedDeviceClass::TrackedDeviceClass_GenericTracker;
		int cntrl1Idx = -1;
		int cntrl2Idx = -1;
		int trkr1Idx = -1;
		int trkr2Idx = -1;
	};


	struct DeviceInfo {
		std::string serial;
		vr::ETrackedDeviceClass deviceClass = vr::TrackedDeviceClass_Invalid;
		uint32_t openvrId = 0;
		uint32_t renderModelIndex = 0;
		vr::VROverlayHandle_t renderModelOverlay = vr::k_ulOverlayHandleInvalid;
		std::string renderModelOverlayName;
		bool isTracked = false;
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
		arma::rowvec modelCNTRL1;
		arma::rowvec modelCNTRL2;

		int currentProfileIdx = 0;

		std::string model_file_type = ".csv";
		std::string model_file_name = "default";

		vr::HmdVector3d_t lastHmdPos = { 0, 0, 0 };
		vr::HmdVector3d_t lastHmdRot = { 0, 0, 0 };

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
		bool useTrackers = true;
		bool trackHMDVel = true;
		bool trackHMDRot = true;
		bool validTRKRSample = false;
		bool useContDirForStraf = false;
		bool useContDirForRev = false;
		bool pressedFlag = false;
		bool inputStateChanged = false;

		int gameType = 0;
		int hmdType = 0;
		int buttonControlSelect = 0;
		int controlSelectOverlayHandle = -1;
		int vive_controller_model_index = -1;
		int disableButton = -1;
		int unnTouchedCount = 0;
		int lastValidHMDSampleMKi = 0;
		int lastCNTRLSampleMKi = 0;
		int lastValidTRKRSampleMKi = 0;

		static const int HMD_Y_VEL_IDX = 0;
		static const int CNTRL1_Y_VEL_IDX = 1;
		static const int CNTRL2_Y_VEL_IDX = 2;
		static const int TRKR1_Y_VEL_IDX = 3;
		static const int TRKR2_Y_VEL_IDX = 4;
		static const int HMD_YAW_VEL_IDX = 5;
		static const int HMD_PITCH_VEL_IDX = 6;
		static const int HMD_X_VEL_IDX = 7;
		static const int HMD_Z_VEL_IDX = 8;
		static const int TOUCH_VAL_IDX = 9;

		int maxSNHMD = 16;
		int startSNHMD = 12;
		int reqSNHMD = 8;
		int maxSNTRKR = 31;
		int startSNTRKR = 20;
		int reqSNTRKR = 10;
		int maxSNCNTRL = 22;
		int reqSNCNTRL = 20;

		uint64_t controller1ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t controller2ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t controlUsedID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t hmdID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t tracker1ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t tracker2ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t ovrwipCNTRLID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t directionDevice = vr::k_unTrackedDeviceIndexInvalid;

		float hmdVelVariance = 0.07;
		float hmdMinDVPerSN = 0.75;
		float trkrVariance = 0.07;
		float cntrlVariance = 0.07;
		float hmdMaxPROTVel = 0;
		float hmdMaxYROTVel = 0;
		float hmdMaxXVel = 0;
		float hmdMaxZVel = 0;
		float sNValidTouch = 0;
		float minTouch = 0.35;
		float midTouch = 0.7;
		float maxTouch = 1.0;
		float touchX = 0;
		float touchY = 0;
		float hmdYaw = 0;
		float contYaw = 0;
		float contRoll = 0;
		float contPitch = 0;

		double timeStep = 1.0 / 40.0;
		double dT = timeStep * 1000;
		double timeLastTick = 0.0;
		double timeLastCNTRLSN = 0.0;
		double identifyControlLastTime = 99999;
		double identifyControlTimeOut = 6000;
		double timeLastGraphPoint = 0.0;

		double currentRunTimeInSec = 0.0;

		std::pair<float, int> computeSNDelta(arma::mat sN, arma::mat mN);
		std::pair<float, int> computeSNDeltaOffset(arma::mat sN, arma::mat mN);
		std::pair<int, int> computeSNDV(arma::mat sN, arma::mat mN);
		int findMNIdxGTS(float s, arma::mat mN);

		void loadDataModel();
		void clearSamplesAndModel();

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
		Q_INVOKABLE bool getTrackHMDRot();
		Q_INVOKABLE float getWalkTouch();
		Q_INVOKABLE float getJogTouch();
		Q_INVOKABLE float getRunTouch();
		Q_INVOKABLE bool getUseButtonAsToggle();
		Q_INVOKABLE bool getButtonEnables();
		Q_INVOKABLE bool getDeviceEnabled(int devClass, int devIdx, int mode);
		Q_INVOKABLE bool isWIPEnabled();
		Q_INVOKABLE bool isStepDetected();
		Q_INVOKABLE QList<qreal> getGraphPoses(double tdiff);
		Q_INVOKABLE QList<qreal> getHMDSample();
		Q_INVOKABLE QList<qreal> getCNTRLSample(); 
		Q_INVOKABLE QList<qreal> getTRKRSample();
		Q_INVOKABLE void applyVirtualStep();
		Q_INVOKABLE QList<qreal> getModelData();
		Q_INVOKABLE QList<qreal> trainingDataSample(float scaleSpeed, double tdiff);
		Q_INVOKABLE void completeTraining();

		Q_INVOKABLE unsigned getProfileCount();
		Q_INVOKABLE QString getProfileName(unsigned index);

		Q_INVOKABLE QList<QString> getDataModelNames();

	public slots:
		void enableWIP(bool enable);
		void setTrackHMDVel(bool val);
		void setTrackHMDRot(bool val);
		void setDirectionDevice(int choice);
		void enableDevice(int deviceClass, int devIdx, bool enable, int mode);
		void setDisableButton(int buttonId);
		void setButtonEnables(bool val);
		void setHMDMaxVari(float val);
		void setMinTouch(float value);
		void setMidTouch(float value);
		void setMaxTouch(float value);
		void setGameStepType(int gameType);
		void setHMDType(int gameType);
		void setButtonControlSelect(int control);
		void setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex, float r, float g, float b, float sx, float sy, float sz);
		void runSampleOnModel();

		bool buttonStatus();

		void stopMovement();
		void stopClickMovement();
		void applyAxisMovement(vr::VRControllerAxis_t axisState);
		void applyClickMovement();
		void applyGripMovement();
		void applyKeyMovement();

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
