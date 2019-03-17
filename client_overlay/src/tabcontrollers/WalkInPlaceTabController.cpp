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
		linStat = LinearAnalyzer();
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
						deviceInfos.push_back(info);
						if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {
							if (hmdID == vr::k_unTrackedDeviceIndexInvalid) {
								hmdID = info->openvrId;
								/*try {
									vrwalkinplace::VRWalkInPlace vrwalkinplace;
									vrwalkinplace.connect();
									vrwalkinplace.openvrDeviceAdded(hmdID);
								}
								catch (std::exception& e) {
									LOG(INFO) << "Exception caught while adding hmd to driver: " << e.what();
								}*/
							}
						}
						else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
							if (info->serial.find("ovrwip") == std::string::npos) {
								if (controller1ID == vr::k_unTrackedDeviceIndexInvalid) {
									controller1ID = info->openvrId;
									LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
								}
								else if (controller2ID == vr::k_unTrackedDeviceIndexInvalid && info->openvrId != controller1ID) {
									controller2ID = info->openvrId;
									LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
								}
							}
							else {
								LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
								vrLocoContID = info->openvrId;
							}
						}
						else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker) {
							if (tracker1ID == vr::k_unTrackedDeviceIndexInvalid) {
								tracker1ID = info->openvrId;
								LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
							}
							else if (tracker2ID == vr::k_unTrackedDeviceIndexInvalid && info->openvrId != tracker1ID) {
								tracker2ID = info->openvrId;
								LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
							}
						}
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
		if (!initializedDataModel) {
			if (!dataTrainingRequired) {
				try {
					bool loaded = dataModel.load(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + "/" + lr_file_name + lr_file_type);
					if (loaded) {
						//dataModel = dataModel.t();
						initializedDataModel = true;
						dataTrainingRequired = false;
						LOG(INFO) << " loaded model file on tick";
					}
					else {
						dataTrainingRequired = true;
						dataModel.insert_cols(0, 1);
						dataModel.insert_rows(0, 6);
						dataSample.insert_cols(0, 1);
						dataSample.insert_rows(0, 6);
						LOG(INFO) << "unable to load model file on tick";
					}
				}
				catch (std::exception& e) {
					initializedDataModel = false;
					dataTrainingRequired = true;
					LOG(INFO) << "Exception caught while loading data model: " << e.what();
				}
			}
		}
		else if (stepDetectEnabled) {
			if (!initializedDriver) {
				if (controller2ID != vr::k_unTrackedDeviceIndexInvalid && controller1ID != vr::k_unTrackedDeviceIndexInvalid) {
					try {
						vrwalkinplace::VRWalkInPlace vrwalkinplace;
						vrwalkinplace.connect();
						vrwalkinplace.openvrEnableDriver(true);
						initializedDriver = true;
					}
					catch (std::exception& e) {
						LOG(INFO) << "Exception caught while adding initializing driver: " << e.what();
					}
				}
			}
			else {
				applyStepPoseDetect();
			}
		}
		if (identifyControlTimerSet) {
			auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			auto tdiff = ((double)(now - identifyControlLastTime));
			//LOG(INFO) << "DT: " << tdiff;
			if (tdiff >= identifyControlTimeOut) {
				identifyControlTimerSet = false;
				try {
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
				catch (std::exception& e) {
					LOG(INFO) << "Exception caught while finding vive controller model: " << e.what();
				}
				setDeviceRenderModel(controlSelectOverlayHandle, vive_controller_model_index, 1, 1, 1, 1, 1, 1);
			}
		}
		if (settingsUpdateCounter >= 45) {
			settingsUpdateCounter = 0;
			if (hmdID == vr::k_unTrackedDeviceIndexInvalid ||
				(controller1ID == vr::k_unTrackedDeviceIndexInvalid || controller2ID == vr::k_unTrackedDeviceIndexInvalid) ||
				(useTrackers && (tracker1ID == vr::k_unTrackedDeviceIndexInvalid || tracker2ID == vr::k_unTrackedDeviceIndexInvalid))) {
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
							deviceInfos.push_back(info);
							if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {
								if (hmdID == vr::k_unTrackedDeviceIndexInvalid) {
									hmdID = info->openvrId;
								}
							}
							else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
								if (info->serial.find("ovrwip") == std::string::npos) {
									if (controller1ID == vr::k_unTrackedDeviceIndexInvalid) {
										controller1ID = info->openvrId;
										LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
									}
									else if (controller2ID == vr::k_unTrackedDeviceIndexInvalid && info->openvrId != controller1ID) {
										controller2ID = info->openvrId;
										LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
									}
								}
								else {
									LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
									vrLocoContID = info->openvrId;
								}
							}
							else if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker) {
								if (tracker1ID == vr::k_unTrackedDeviceIndexInvalid) {
									tracker1ID = info->openvrId;
									LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
								}
								else if (tracker2ID == vr::k_unTrackedDeviceIndexInvalid && info->openvrId != tracker1ID) {
									tracker2ID = info->openvrId;
									LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
								}
							}
						}
						maxValidDeviceId = id;
					}
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

	bool WalkInPlaceTabController::isStepDetectionEnabled() {
		return stepDetectEnabled;
	}

	double WalkInPlaceTabController::getStepTime() {
		return 0;
	}

	int WalkInPlaceTabController::getGameType() {
		return gameType;
	}

	int WalkInPlaceTabController::getHMDType() {
		return hmdType;
	}

	int WalkInPlaceTabController::getButtonControlSelect() {
		return buttonControlSelect;
	}

	int WalkInPlaceTabController::getDisableButton() {
		return disableButton;
	}

	bool WalkInPlaceTabController::getUseButtonAsToggle() {
		return buttonAsToggle;
	}

	bool WalkInPlaceTabController::getButtonEnables() {
		return buttonEnables;
	}

	bool WalkInPlaceTabController::getUseTrackers() {
		return useTrackers;
	}

	bool WalkInPlaceTabController::getDisableHMD() {
		return disableHMD;
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

	bool WalkInPlaceTabController::getUseContDirForStraf() {
		return useContDirForStraf;
	}

	bool WalkInPlaceTabController::getUseContDirForRev() {
		return useContDirForRev;
	}


	void WalkInPlaceTabController::setupStepGraph() {
	}

	QList<qreal> WalkInPlaceTabController::getModelData() {
		bool loaded = initializedDataModel;
		if (!initializedDataModel) {
			try {
				bool loaded = dataModel.load(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + "/" + lr_file_name + lr_file_type);
				if (loaded) {
					//dataModel = dataModel.t();
					LOG(INFO) << " loaded model file on display model";
					initializedDataModel = true;
					dataTrainingRequired = false;
				}
				else {
					dataTrainingRequired = true;
					dataModel.insert_cols(0, 1);
					dataModel.insert_rows(0, 6);
					dataSample.insert_cols(0, 1);
					dataSample.insert_rows(0, 6);
					LOG(INFO) << "unable to load model file on display model";
				}
			}
			catch (std::exception& e) {
				initializedDataModel = false;
				dataTrainingRequired = true;
				LOG(INFO) << "Exception caught while loading data model: " << e.what();
			}
		}

		QList<qreal> vals;
		if (loaded) {
			try {
				for (int i = 0; i < dataModel.n_cols; i++) {
					for (int j = 0; j < 5; j++) { // just the hmd, cntrl, tracker Y vel
						if (j < dataModel.n_rows) {
							vals.push_back((float)dataModel(j, i));
						}
					}
					vals.push_back(dataModel(dataModel.n_rows - 1, i)); //touch val last val in all models
				}
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while getting the data model: " << e.what();
			}
		}

		return vals;
	}

	void WalkInPlaceTabController::completeLRTraining() {
		try {
			dataModel = dataModel.t();
			mlpack::data::Save(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + "/" + lr_file_name + lr_file_type, dataModel);
			dataModel.clear();
			initializedDataModel = false;
			dataTrainingRequired = false;
			addWalkInPlaceProfile(QString(currentProfileName.c_str()));
		}
		catch (std::exception& e) {
			LOG(INFO) << "Exception caught while saving data model: " << e.what();
		}
	}

	QList<qreal> WalkInPlaceTabController::lrTrainingDataSample(float scaleSpeed, double tdiff) {
		initializedDataModel = false;
		dataTrainingRequired = true;
		vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
		bool firstController = true;
		bool firstTracker = true;
		vr::HmdVector3d_t hmdVel = { 0, 0, 0 };
		vr::HmdVector3d_t hmdAcc = { 0, 0, 0 };
		vr::HmdVector3d_t hmdRotVel = { 0, 0, 0 };
		vr::HmdVector3d_t cont1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t cont1Acc = { 0, 0, 0 };
		vr::HmdVector3d_t cont1RotVel = { 0, 0, 0 };
		vr::HmdVector3d_t cont2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t cont2Acc = { 0, 0, 0 };
		vr::HmdVector3d_t cont2RotVel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker1Acc = { 0, 0, 0 };
		vr::HmdVector3d_t tracker1RotVel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker2Acc = { 0, 0, 0 };
		vr::HmdVector3d_t tracker2RotVel = { 0, 0, 0 };
		if (hmdType != 0) {
			auto m = latestDevicePoses[hmdID].mDeviceToAbsoluteTracking.m;

			hmdVel.v[0] = (m[0][3] - lastHmdPos.v[0]) / tdiff;
			hmdVel.v[1] = (m[1][3] - lastHmdPos.v[1]) / tdiff;
			hmdVel.v[2] = (m[2][3] - lastHmdPos.v[2]) / tdiff;

			lastHmdPos.v[0] = m[0][3];
			lastHmdPos.v[1] = m[1][3];
			lastHmdPos.v[2] = m[2][3];

		}
		else {
			hmdVel.v[0] = latestDevicePoses[hmdID].vVelocity.v[0];
			hmdVel.v[1] = latestDevicePoses[hmdID].vVelocity.v[1];
			hmdVel.v[2] = latestDevicePoses[hmdID].vVelocity.v[2];
		}

		vr::HmdQuaternion_t qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[hmdID].mDeviceToAbsoluteTracking);
		vr::HmdVector3d_t forward = { 0,0,-1 };
		vr::HmdVector3d_t right = { 1,0,0 };
		vr::HmdVector3d_t devForward = vrmath::quaternionRotateVector(qRotation, forward);
		vr::HmdVector3d_t devRight = vrmath::quaternionRotateVector(qRotation, right);

		//hmdVel = vrmath::quaternionRotateVector(qRotation, hmdVel);

		double devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
		double devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
		double devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

		hmdRotVel.v[0] = (devYaw - lastHmdRot.v[0]) / tdiff;
		hmdRotVel.v[1] = (devPitch - lastHmdRot.v[1]) / tdiff;
		hmdRotVel.v[2] = (devRoll - lastHmdRot.v[2]) / tdiff;

		hmdAcc.v[0] = (hmdVel.v[0] - lastHmdVel.v[0]) / tdiff;
		hmdAcc.v[1] = (hmdVel.v[1] - lastHmdVel.v[1]) / tdiff;
		hmdAcc.v[2] = (hmdVel.v[2] - lastHmdVel.v[2]) / tdiff;

		lastHmdVel.v[0] = hmdVel.v[0];
		lastHmdVel.v[1] = hmdVel.v[1];
		lastHmdVel.v[2] = hmdVel.v[2];

		cont1Vel.v[0] = latestDevicePoses[controller1ID].vVelocity.v[0];
		cont1Vel.v[1] = latestDevicePoses[controller1ID].vVelocity.v[1];
		cont1Vel.v[2] = latestDevicePoses[controller1ID].vVelocity.v[2];
		qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[controller1ID].mDeviceToAbsoluteTracking);
		devForward = vrmath::quaternionRotateVector(qRotation, forward);
		devRight = vrmath::quaternionRotateVector(qRotation, right);

		//cont1Vel = vrmath::quaternionRotateVector(qRotation, cont1Vel);

		devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
		devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
		devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

		cont1RotVel.v[0] = (devYaw - lastCont1Rot.v[0]) / tdiff;
		cont1RotVel.v[1] = (devPitch - lastCont1Rot.v[1]) / tdiff;
		cont1RotVel.v[2] = (devRoll - lastCont1Rot.v[2]) / tdiff;

		cont1Acc.v[0] = (cont1Vel.v[0] - lastCont1Vel.v[0]) / tdiff;
		cont1Acc.v[1] = (cont1Vel.v[1] - lastCont1Vel.v[1]) / tdiff;
		cont1Acc.v[2] = (cont1Vel.v[2] - lastCont1Vel.v[2]) / tdiff;

		lastCont1Vel.v[0] = cont1Vel.v[0];
		lastCont1Vel.v[1] = cont1Vel.v[1];
		lastCont1Vel.v[2] = cont1Vel.v[2];

		cont2Vel.v[0] = latestDevicePoses[controller2ID].vVelocity.v[0];
		cont2Vel.v[1] = latestDevicePoses[controller2ID].vVelocity.v[1];
		cont2Vel.v[2] = latestDevicePoses[controller2ID].vVelocity.v[2];
		qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[controller2ID].mDeviceToAbsoluteTracking);
		devForward = vrmath::quaternionRotateVector(qRotation, forward);
		devRight = vrmath::quaternionRotateVector(qRotation, right);

		//cont2Vel = vrmath::quaternionRotateVector(qRotation, cont2Vel);

		devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
		devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
		devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

		cont2RotVel.v[0] = (devYaw - lastCont2Rot.v[0]) / tdiff;
		cont2RotVel.v[1] = (devPitch - lastCont2Rot.v[1]) / tdiff;
		cont2RotVel.v[2] = (devRoll - lastCont2Rot.v[2]) / tdiff;

		cont2Acc.v[0] = (cont2Vel.v[0] - lastCont2Vel.v[0]) / tdiff;
		cont2Acc.v[1] = (cont2Vel.v[1] - lastCont2Vel.v[1]) / tdiff;
		cont2Acc.v[2] = (cont2Vel.v[2] - lastCont2Vel.v[2]) / tdiff;

		lastCont2Vel.v[0] = cont2Vel.v[0];
		lastCont2Vel.v[1] = cont2Vel.v[1];
		lastCont2Vel.v[2] = cont2Vel.v[2];

		if (useTrackers) {
			if (tracker1ID != vr::k_unTrackedDeviceIndexInvalid) {
				tracker1Vel.v[0] = latestDevicePoses[tracker1ID].vVelocity.v[0];
				tracker1Vel.v[1] = latestDevicePoses[tracker1ID].vVelocity.v[1];
				tracker1Vel.v[2] = latestDevicePoses[tracker1ID].vVelocity.v[2];
				qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[tracker1ID].mDeviceToAbsoluteTracking);
				devForward = vrmath::quaternionRotateVector(qRotation, forward);
				devRight = vrmath::quaternionRotateVector(qRotation, right);

				//tracker1Vel = vrmath::quaternionRotateVector(qRotation, tracker1Vel);

				devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
				devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
				devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

				tracker1RotVel.v[0] = (devYaw - lastTracker1Rot.v[0]) / tdiff;
				tracker1RotVel.v[1] = (devPitch - lastTracker1Rot.v[1]) / tdiff;
				tracker1RotVel.v[2] = (devRoll - lastTracker1Rot.v[2]) / tdiff;
			}
			if (tracker2ID != vr::k_unTrackedDeviceIndexInvalid) {
				tracker2Vel.v[0] = latestDevicePoses[tracker2ID].vVelocity.v[0];
				tracker2Vel.v[1] = latestDevicePoses[tracker2ID].vVelocity.v[1];
				tracker2Vel.v[2] = latestDevicePoses[tracker2ID].vVelocity.v[2];
				qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[tracker2ID].mDeviceToAbsoluteTracking);
				devForward = vrmath::quaternionRotateVector(qRotation, forward);
				devRight = vrmath::quaternionRotateVector(qRotation, right);

				//tracker2Vel = vrmath::quaternionRotateVector(qRotation, tracker2Vel);

				devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
				devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
				devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

				tracker2RotVel.v[0] = (devYaw - lastTracker2Rot.v[0]) / tdiff;
				tracker2RotVel.v[0] = (devPitch - lastTracker2Rot.v[1]) / tdiff;
				tracker2RotVel.v[0] = (devRoll - lastTracker2Rot.v[2]) / tdiff;
			}
		}
		try {
			int n = dataModel.n_cols;
			dataModel.insert_cols(n, 1);
			//dataModel(0, n) = hmdVel.v[0];
			dataModel(0, n) = hmdVel.v[1];
			//dataModel(2, n) = hmdVel.v[2];
			dataModel(1, n) = cont1Vel.v[1];
			dataModel(2, n) = cont2Vel.v[1];
			dataModel(3, n) = tracker1Vel.v[1];
			dataModel(4, n) = tracker2Vel.v[1];
			dataModel(5, n) = scaleSpeed;
		}
		catch (std::exception& e) {
			LOG(INFO) << "Exception caught while building data model: " << e.what();
		}

		QList<qreal> vals;
		vals.push_back(hmdVel.v[0]);
		vals.push_back(hmdVel.v[1]);
		vals.push_back(hmdVel.v[2]);

		vals.push_back(cont1Vel.v[0]);
		vals.push_back(cont1Vel.v[1]);
		vals.push_back(cont1Vel.v[2]);
		//vals.push_back(hmdPitchAngVel);

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
		if (stepPoseDetected) {
			if (useTrackers && disableHMD) {
				vals.push_back(0);
				vals.push_back(1);
			}
			else {
				vals.push_back(1);
				if (useTrackers) {
					vals.push_back(1);
				}
				else {
					vals.push_back(0);
				}
			}
		}
		else {
			vals.push_back(0);
			vals.push_back(0);
		}
		vals.push_back(0); // run
		vals.push_back(0); // jog

		return vals;
	}

	QList<qreal> WalkInPlaceTabController::getGraphPoses() {
		if (!stepDetectEnabled) {
			vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
		}
		auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		bool firstController = true;
		bool firstTracker = true;
		vr::HmdVector3d_t hmdVel = { 0, 0, 0 };
		vr::HmdVector3d_t hmdRotVel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker1RotVel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker2RotVel = { 0, 0, 0 };
		vr::HmdVector3d_t cont1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t cont1RotVel = { 0, 0, 0 };
		vr::HmdVector3d_t cont2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t cont2RotVel = { 0, 0, 0 };
		for (auto info : deviceInfos) {
			if (latestDevicePoses[info->openvrId].bPoseIsValid) {
				if (info->deviceClass == vr::TrackedDeviceClass_HMD) {
					if (hmdType != 0) {
						auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						double tdiff = ((double)(now - timeLastTick)) / 1000.0;
						timeLastTick = now;
						auto m = latestDevicePoses[info->openvrId].mDeviceToAbsoluteTracking.m;

						hmdVel.v[0] = (m[0][3] - lastHmdPos.v[0]) / tdiff;
						hmdVel.v[1] = (m[1][3] - lastHmdPos.v[1]) / tdiff;
						hmdVel.v[2] = (m[2][3] - lastHmdPos.v[2]) / tdiff;

						lastHmdPos.v[0] = m[0][3];
						lastHmdPos.v[1] = m[1][3];
						lastHmdPos.v[2] = m[2][3];

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
					if (info->serial.find("ovrwip") == std::string::npos) {
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
		if (stepPoseDetected) {
			if (useTrackers && disableHMD) {
				vals.push_back(0);
				vals.push_back(1);
			}
			else {
				vals.push_back(1);
				if (useTrackers) {
					vals.push_back(1);
				}
				else {
					vals.push_back(0);
				}
			}
		}
		else {
			vals.push_back(0);
			vals.push_back(0);
		}
		vals.push_back(0); // run
		vals.push_back(0); // jog

		return vals;

	}

	bool WalkInPlaceTabController::isStepDetected() {
		return stepPoseDetected;
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
			entry.buttonControlSelect = settings->value("buttonControlSelect", 0).toInt();
			entry.useTrackers = settings->value("useTrackers", false).toBool();
			entry.disableHMD = settings->value("disableHMD", false).toBool();
			entry.useContDirForStraf = settings->value("useContDirForStraf", false).toBool();
			entry.useContDirForRev = settings->value("useContDirForRev", false).toBool();
			entry.stepTime = settings->value("stepTime", 0.5).toDouble();
			entry.disableButton = settings->value("disableButton", 0).toInt();
			entry.walkTouch = settings->value("walkTouch", 0.6).toFloat();
			entry.jogTouch = settings->value("jogTouch", 0.87).toFloat();
			entry.runTouch = settings->value("runTouch", 1).toFloat();
			entry.buttonAsToggle = settings->value("buttonAsToggle", false).toBool();
			entry.buttonEnables = settings->value("buttonEnables", false).toBool();
			entry.modelFile = settings->value("modelFile", QString((lr_file_name + lr_file_type).c_str())).toString().toStdString();
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
			settings->setValue("buttonControlSelect", p.buttonControlSelect);
			settings->setValue("useTrackers", p.useTrackers);
			settings->setValue("disableHMD", p.disableHMD);
			settings->setValue("useContDirForStraf", p.useContDirForStraf);
			settings->setValue("useContDirForRev", p.useContDirForRev);
			settings->setValue("stepTime", p.stepTime);
			settings->setValue("disableButton", p.disableButton);
			settings->setValue("walkTouch", p.walkTouch);
			settings->setValue("jogTouch", p.jogTouch);
			settings->setValue("runTouch", p.runTouch);
			settings->setValue("buttonAsToggle", p.buttonAsToggle);
			settings->setValue("buttonEnables", p.buttonEnables);
			settings->setValue("modelFile", QString::fromStdString(p.modelFile));
			settings->setValue("colCount", p.modelColCount);
			settings->setValue("rowCount", p.modelRowCount);
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
		lr_file_name = name.toStdString();
		profile->profileName = name.toStdString();
		currentProfileName = name.toStdString();
		profile->stepDetectionEnabled = isStepDetectionEnabled();
		profile->gameType = gameType;
		profile->hmdType = hmdType;
		profile->buttonControlSelect = buttonControlSelect;
		profile->useTrackers = useTrackers || disableHMD;
		profile->disableHMD = disableHMD;
		profile->useContDirForStraf = useContDirForStraf;
		profile->useContDirForRev = useContDirForRev;
		profile->disableButton = disableButton;
		profile->walkTouch = walkTouch;
		profile->jogTouch = jogTouch;
		profile->runTouch = runTouch;
		profile->buttonAsToggle = buttonAsToggle;
		profile->buttonEnables = buttonEnables;
		profile->modelColCount = modelColCount;
		profile->modelRowCount = modelRowCount;
		saveWalkInPlaceProfiles();
		OverlayController::appSettings()->sync();
		emit walkInPlaceProfilesChanged();
	}

	void WalkInPlaceTabController::applyWalkInPlaceProfile(unsigned index) {
		if (index < walkInPlaceProfiles.size()) {
			auto& profile = walkInPlaceProfiles[index];
			lr_file_name = profile.profileName;
			dataModel.clear();
			dataTrainingRequired = false;
			initializedDataModel = false;
			gameType = profile.gameType;
			hmdType = profile.hmdType;
			buttonControlSelect = profile.buttonControlSelect;
			useTrackers = profile.useTrackers || profile.disableHMD;
			disableHMD = profile.disableHMD;
			useContDirForStraf = profile.useContDirForStraf;
			useContDirForRev = profile.useContDirForRev;
			disableButton = profile.disableButton;
			walkTouch = profile.walkTouch;
			jogTouch = profile.jogTouch;
			runTouch = profile.runTouch;
			buttonAsToggle = profile.buttonAsToggle;
			buttonEnables = profile.buttonEnables;

			enableStepDetection(profile.stepDetectionEnabled);
			setGameStepType(profile.gameType);
			setHMDType(profile.hmdType);
			setButtonControlSelect(profile.buttonControlSelect);
			setUseTrackers(profile.useTrackers || profile.disableHMD);
			setDisableHMD(profile.disableHMD);
			setUseContDirForStraf(profile.useContDirForStraf);
			setUseContDirForRev(profile.useContDirForRev);
			setStepTime(profile.stepTime);
			setDisableButton(profile.disableButton);
			setButtonAsToggle(profile.buttonAsToggle);
			setButtonEnables(profile.buttonEnables);
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
		/*if (!initializedDriver && enable && (controller2ID != vr::k_unTrackedDeviceIndexInvalid && controller1ID != vr::k_unTrackedDeviceIndexInvalid)) {
			try {
				vrwalkinplace::VRWalkInPlace vrwalkinplace;
				vrwalkinplace.connect();
				vrwalkinplace.openvrEnableDriver(true);
				initializedDriver = true;
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while adding initializing driver: " << e.what();
			}
		}*/
	}

	void WalkInPlaceTabController::setStepTime(double value) {

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

	void WalkInPlaceTabController::setUseContDirForStraf(bool val) {
		useContDirForStraf = val;
	}

	void WalkInPlaceTabController::setUseContDirForRev(bool val) {
		useContDirForRev = val;
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

	void WalkInPlaceTabController::setDisableButton(int buttonId) {
		disableButton = buttonId;
	}

	void WalkInPlaceTabController::disableController(bool val, int control) {

	}

	void WalkInPlaceTabController::disableTracker(bool val, int control) {

	}

	void WalkInPlaceTabController::updateButtonState(uint32_t deviceId, bool firstController) {
		if (deviceId != vr::k_unTrackedDeviceIndexInvalid && (buttonControlSelect >= 2 || (firstController == buttonControlSelect == 0))) {
			vr::VRControllerState_t state;
			vr::VRSystem()->GetControllerState(deviceId, &state, sizeof(state));
			//LOG(INFO) << "Check accuracy button : " << deviceId << " : " << g_AccuracyButton << " : " << state.ulButtonPressed << " : " << vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger);
			//LOG(INFO) << "current button : " << state.ulButtonPressed;
			bool isHoldingButton = false;
			vr::EVRButtonId buttonUsed;
			switch (disableButton) {
			case 0:
				buttonUsed = vr::EVRButtonId::k_EButton_Grip;
				break;
			case 1:
				buttonUsed = vr::EVRButtonId::k_EButton_SteamVR_Trigger;
				break;
			default:
				break;
			}
			switch (buttonUsed) {
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
			case vr::EVRButtonId::k_EButton_SteamVR_Trigger:
				if (state.ulButtonPressed& vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) {
					isHoldingButton = true;
				}
				if (state.ulButtonTouched& vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) {
					isHoldingButton = true;
				}
				break;
			default:
				break;
			}
			if ((buttonControlSelect == 0 && firstController && isHoldingButton) ||
				(buttonControlSelect == 1 && !firstController && isHoldingButton) ||
				(buttonControlSelect >= 2 && isHoldingButton)) {
				holdingButton = true;
			}
			if ((buttonControlSelect == 0 && firstController && !isHoldingButton) ||
				(buttonControlSelect == 1 && !firstController && !isHoldingButton)) {
				holdingButton = false;
			}
		}
	}

	bool WalkInPlaceTabController::buttonStatus() {
		return (disableButton == 2 || (holdingButton && buttonEnables) || (!holdingButton && !buttonEnables));
	}

	void WalkInPlaceTabController::setButtonAsToggle(bool val) {
		buttonAsToggle = val;
	}

	void WalkInPlaceTabController::setButtonEnables(bool val) {
		buttonEnables = val;
	}

	void WalkInPlaceTabController::setGameStepType(int type) {
		gameType = type;
	}

	void WalkInPlaceTabController::setHMDType(int type) {
		hmdType = type;
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

	void WalkInPlaceTabController::setButtonControlSelect(int control) {
		buttonControlSelect = control;
		if ((control == 0 && controller1ID != vr::k_unTrackedDeviceIndexInvalid) ||
			(control == 1 && controller2ID != vr::k_unTrackedDeviceIndexInvalid)) {
			if (!identifyControlTimerSet) {
				identifyControlTimerSet = true;
				identifyControlLastTime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				controlSelectOverlayHandle = 999;
				for (int d = 0; d < deviceInfos.size(); d++) {
					if (deviceInfos[d]->openvrId == (control == 0 ? controller1ID : controller2ID)) {
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

	void WalkInPlaceTabController::applyStepPoseDetect() {
		if (controller1ID != vr::k_unTrackedDeviceIndexInvalid && controller2ID != vr::k_unTrackedDeviceIndexInvalid) {
			float samplePerSec = 45.0;
			double deltatime = 1.0 / samplePerSec * 1000;
			auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			double tdiff = ((double)(now - timeLastTick));
			bool hmdStep = false;
			//LOG(INFO) << "DT: " << tdiff;
			if (tdiff >= deltatime) {
				timeLastTick = now;
				if (disableButton == 0 || disableButton == 1) {
					holdingButton = false;
					if (controller1ID != vr::k_unTrackedDeviceIndexInvalid) {
						updateButtonState(controller1ID, true);
					}
					if (controller2ID != vr::k_unTrackedDeviceIndexInvalid) {
						updateButtonState(controller2ID, false);
					}
				}

				vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
				vr::HmdVector3d_t hmdVel = { 0, 0, 0 };
				vr::HmdVector3d_t hmdAcc = { 0, 0, 0 };
				vr::HmdVector3d_t hmdRotVel = { 0, 0, 0 };
				vr::HmdVector3d_t cont1Vel = { 0, 0, 0 };
				vr::HmdVector3d_t cont1Acc = { 0, 0, 0 };
				vr::HmdVector3d_t cont1RotVel = { 0, 0, 0 };
				vr::HmdVector3d_t cont2Vel = { 0, 0, 0 };
				vr::HmdVector3d_t cont2Acc = { 0, 0, 0 };
				vr::HmdVector3d_t cont2RotVel = { 0, 0, 0 };
				vr::HmdVector3d_t tracker1Vel = { 0, 0, 0 };
				vr::HmdVector3d_t tracker1Acc = { 0, 0, 0 };
				vr::HmdVector3d_t tracker1RotVel = { 0, 0, 0 };
				vr::HmdVector3d_t tracker2Vel = { 0, 0, 0 };
				vr::HmdVector3d_t tracker2Acc = { 0, 0, 0 };
				vr::HmdVector3d_t tracker2RotVel = { 0, 0, 0 };
				if (hmdType != 0) {
					auto m = latestDevicePoses[hmdID].mDeviceToAbsoluteTracking.m;

					hmdVel.v[0] = (m[0][3] - lastHmdPos.v[0]) / tdiff;
					hmdVel.v[1] = (m[1][3] - lastHmdPos.v[1]) / tdiff;
					hmdVel.v[2] = (m[2][3] - lastHmdPos.v[2]) / tdiff;

					lastHmdPos.v[0] = m[0][3];
					lastHmdPos.v[1] = m[1][3];
					lastHmdPos.v[2] = m[2][3];

				}
				else {
					hmdVel.v[0] = latestDevicePoses[hmdID].vVelocity.v[0];
					hmdVel.v[1] = latestDevicePoses[hmdID].vVelocity.v[1];
					hmdVel.v[2] = latestDevicePoses[hmdID].vVelocity.v[2];
				}

				vr::HmdQuaternion_t qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[hmdID].mDeviceToAbsoluteTracking);
				vr::HmdVector3d_t forward = { 0,0,-1 };
				vr::HmdVector3d_t right = { 1,0,0 };
				vr::HmdVector3d_t devForward = vrmath::quaternionRotateVector(qRotation, forward);
				vr::HmdVector3d_t devRight = vrmath::quaternionRotateVector(qRotation, right);

				//hmdVel = vrmath::quaternionRotateVector(qRotation, hmdVel);

				double devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
				double devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
				double devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

				hmdRotVel.v[0] = (devYaw - lastHmdRot.v[0]) / tdiff;
				hmdRotVel.v[1] = (devPitch - lastHmdRot.v[1]) / tdiff;
				hmdRotVel.v[2] = (devRoll - lastHmdRot.v[2]) / tdiff;

				hmdAcc.v[0] = (hmdVel.v[0] - lastHmdVel.v[0]) / tdiff;
				hmdAcc.v[1] = (hmdVel.v[1] - lastHmdVel.v[1]) / tdiff;
				hmdAcc.v[2] = (hmdVel.v[2] - lastHmdVel.v[2]) / tdiff;

				lastHmdVel.v[0] = hmdVel.v[0];
				lastHmdVel.v[1] = hmdVel.v[1];
				lastHmdVel.v[2] = hmdVel.v[2];

				cont1Vel.v[0] = latestDevicePoses[controller1ID].vVelocity.v[0];
				cont1Vel.v[1] = latestDevicePoses[controller1ID].vVelocity.v[1];
				cont1Vel.v[2] = latestDevicePoses[controller1ID].vVelocity.v[2];
				qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[controller1ID].mDeviceToAbsoluteTracking);
				devForward = vrmath::quaternionRotateVector(qRotation, forward);
				devRight = vrmath::quaternionRotateVector(qRotation, right);

				//cont1Vel = vrmath::quaternionRotateVector(qRotation, cont1Vel);

				devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
				devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
				devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

				cont1RotVel.v[0] = (devYaw - lastCont1Rot.v[0]) / tdiff;
				cont1RotVel.v[1] = (devPitch - lastCont1Rot.v[1]) / tdiff;
				cont1RotVel.v[2] = (devRoll - lastCont1Rot.v[2]) / tdiff;

				cont1Acc.v[0] = (cont1Vel.v[0] - lastCont1Vel.v[0]) / tdiff;
				cont1Acc.v[1] = (cont1Vel.v[1] - lastCont1Vel.v[1]) / tdiff;
				cont1Acc.v[2] = (cont1Vel.v[2] - lastCont1Vel.v[2]) / tdiff;

				lastCont1Vel.v[0] = cont1Vel.v[0];
				lastCont1Vel.v[1] = cont1Vel.v[1];
				lastCont1Vel.v[2] = cont1Vel.v[2];

				cont2Vel.v[0] = latestDevicePoses[controller2ID].vVelocity.v[0];
				cont2Vel.v[1] = latestDevicePoses[controller2ID].vVelocity.v[1];
				cont2Vel.v[2] = latestDevicePoses[controller2ID].vVelocity.v[2];
				qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[controller2ID].mDeviceToAbsoluteTracking);
				devForward = vrmath::quaternionRotateVector(qRotation, forward);
				devRight = vrmath::quaternionRotateVector(qRotation, right);

				//cont2Vel = vrmath::quaternionRotateVector(qRotation, cont2Vel);

				devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
				devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
				devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

				cont2RotVel.v[0] = (devYaw - lastCont2Rot.v[0]) / tdiff;
				cont2RotVel.v[1] = (devPitch - lastCont2Rot.v[1]) / tdiff;
				cont2RotVel.v[2] = (devRoll - lastCont2Rot.v[2]) / tdiff;

				cont2Acc.v[0] = (cont2Vel.v[0] - lastCont2Vel.v[0]) / tdiff;
				cont2Acc.v[1] = (cont2Vel.v[1] - lastCont2Vel.v[1]) / tdiff;
				cont2Acc.v[2] = (cont2Vel.v[2] - lastCont2Vel.v[2]) / tdiff;

				lastCont2Vel.v[0] = cont2Vel.v[0];
				lastCont2Vel.v[1] = cont2Vel.v[1];
				lastCont2Vel.v[2] = cont2Vel.v[2];

				if (false && useTrackers) {
					if (tracker1ID != vr::k_unTrackedDeviceIndexInvalid) {
						tracker1Vel.v[0] = latestDevicePoses[tracker1ID].vVelocity.v[0];
						tracker1Vel.v[1] = latestDevicePoses[tracker1ID].vVelocity.v[1];
						tracker1Vel.v[2] = latestDevicePoses[tracker1ID].vVelocity.v[2];
						qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[tracker1ID].mDeviceToAbsoluteTracking);
						devForward = vrmath::quaternionRotateVector(qRotation, forward);
						devRight = vrmath::quaternionRotateVector(qRotation, right);

						//tracker1Vel = vrmath::quaternionRotateVector(qRotation, tracker1Vel);

						devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
						devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
						devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

						tracker1RotVel.v[0] = (devYaw - lastTracker1Rot.v[0]) / tdiff;
						tracker1RotVel.v[1] = (devPitch - lastTracker1Rot.v[1]) / tdiff;
						tracker1RotVel.v[2] = (devRoll - lastTracker1Rot.v[2]) / tdiff;
					}
					if (tracker2ID != vr::k_unTrackedDeviceIndexInvalid) {
						tracker2Vel.v[0] = latestDevicePoses[tracker2ID].vVelocity.v[0];
						tracker2Vel.v[1] = latestDevicePoses[tracker2ID].vVelocity.v[1];
						tracker2Vel.v[2] = latestDevicePoses[tracker2ID].vVelocity.v[2];
						qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[tracker2ID].mDeviceToAbsoluteTracking);
						devForward = vrmath::quaternionRotateVector(qRotation, forward);
						devRight = vrmath::quaternionRotateVector(qRotation, right);

						//tracker2Vel = vrmath::quaternionRotateVector(qRotation, tracker2Vel);

						devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
						devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
						devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

						tracker2RotVel.v[0] = (devYaw - lastTracker2Rot.v[0]) / tdiff;
						tracker2RotVel.v[0] = (devPitch - lastTracker2Rot.v[1]) / tdiff;
						tracker2RotVel.v[0] = (devRoll - lastTracker2Rot.v[2]) / tdiff;
					}
				}
				try {

					int kV_set = 3;
					int n = dataSample.n_cols;
					dataSample.insert_cols(n, 1);
					if (n > (samplePerSec/kV_set)*2) {
						dataSample.shed_col(0);
						n = n - 1;
					}
					//dataModel(0, n) = hmdVel.v[0];
					dataSample(0, n) = hmdVel.v[1];
					//dataModel(2, n) = hmdVel.v[2];
					dataSample(1, n) = cont1Vel.v[1];
					dataSample(2, n) = cont2Vel.v[1];
					dataSample(3, n) = tracker1Vel.v[1];
					dataSample(4, n) = tracker2Vel.v[1];

					if (dataSample.n_cols > samplePerSec/kV_set) {
						try {
							//arma::mat dS = linStat.computeDelta(dataSample, dataModel, 4);
							int kV_set = 3;
							arma::rowvec mN = dataModel.row(0);
							arma::rowvec sN = dataSample.row(0);
							//LOG(INFO) << "sN: " << sN.n_cols;
							//LOG(INFO) << "mN: " << mN.n_cols;
							int sNk = std::floor((sN.n_cols - 1) / kV_set);
							//LOG(INFO) << "sNk: " << sNk;
							arma::mat dS = arma::mat(2, kV_set);
							dS.fill(9999);
							for (int ki = 1; ki < sN.n_cols; ki += sNk) {
								//LOG(INFO) << "ki: " << ki;
								arma::rowvec sKi = sN.cols(ki, std::min((ki + sNk), (int)sN.n_cols - 1));
								//LOG(INFO) << "sKi: " << sKi.n_cols;
								for (int mi = 1; mi < mN.n_cols; mi += sNk) {
									if ((mi + ((int)sKi.n_cols - 1)) < mN.n_cols) {
										//LOG(INFO) << "mi: " << mi;
										arma::rowvec mKi = mN.cols(mi, mi + ((int)sKi.n_cols - 1));
										arma::rowvec dKi = arma::abs(mKi) - arma::abs(sKi);
										//LOG(INFO) << "dKi: " << std::floor(ki / sNk);
										float dK_i = std::abs(arma::max(dKi));
										if (dK_i < dS(0, std::floor(ki / sNk))) {
											dS(0, std::floor(ki / sNk)) = (dK_i);
											dS(1, std::floor(ki / sNk)) = (mi);
										}
									}
								}
							}

							//LOG(INFO) << "dS cols: " << dS.n_cols;
							//LOG(INFO) << "dS rows: " << dS.n_rows;
							//LOG(INFO) << "dS0: " << dS.row(0);
							//LOG(INFO) << "dS1: " << dS.row(1);

							arma::rowvec vals = dS.row(0);
							LOG(INFO) << " dS min: " << (float)vals.min();

							float min = arma::min(vals);
							float lastQrtSecMin = 0;// arma::min(vals.tail_cols(std::max(0, (int)(vals.n_cols - (sNk)))));
							if (min < 0.04 && lastQrtSecMin < 0.9 && buttonStatus()) {
								stepPoseDetected = true;
							}
							else if (min >= 0.08 || lastQrtSecMin >= 0.13) {
								stepPoseDetected = false;
								stopMovement(0);
							}
						}
						catch (std::exception& e) {
							LOG(INFO) << "Exception caught while computing delta error: " << e.what();
						}
					}
				}
				catch (std::exception& e) {
					LOG(INFO) << "Exception caught while predicting data: " << e.what();
				}

				timeLastTick = now;
			}
		}
		if (stepPoseDetected) {
			vr::VRControllerAxis_t axisState;
			if (controller1ID != vr::k_unTrackedDeviceIndexInvalid || controller2ID != vr::k_unTrackedDeviceIndexInvalid) {
				if (useContDirForStraf || useContDirForRev) {
					vr::HmdQuaternion_t qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[controller1ID].mDeviceToAbsoluteTracking);
					vr::HmdVector3d_t forward = { 0,0,-1 };
					vr::HmdVector3d_t right = { 1,0,0 };
					vr::HmdVector3d_t forwardRot = vrmath::quaternionRotateVector(qRotation, forward);
					vr::HmdVector3d_t rightRot = vrmath::quaternionRotateVector(qRotation, right);
					float pitch = (180 * std::asin(forwardRot.v[1])) / M_PI;
					float yaw = (180 * std::asin(forwardRot.v[0])) / M_PI;
					float roll = (180 * std::asin(rightRot.v[1])) / M_PI;
					touchX = 0;
					touchY = 1;
					float diffYaw = (hmdYaw - yaw);
					if (useContDirForRev && pitch > 77) {
						touchX = 0;
						touchY = -1;
					}
					else if (useContDirForStraf && pitch < 77 && std::fabs(diffYaw) > 30) { //std::fabs(roll) > 45 ) {
						touchX = std::sin((diffYaw*M_PI) / 180.0);
						touchY = std::cos((diffYaw*M_PI) / 180.0);
					}
					//LOG(INFO) << "CONT Pitch,Yaw,Roll : " << pitch << ",(" << hmdYaw << "-" << yaw << ")=" << diffYaw << "," << roll;
					//LOG(INFO) << "CONT Pitch,Yaw,Roll : " << pitch << "," <<  yaw << "," << roll;
					//LOG(INFO) << "Cont Forward (x,y,z): " << forwardRot.v[0] << "," << forwardRot.v[1] << "," << forwardRot.v[2];
					//LOG(INFO) << "HMD  Forward (x,y,z): " << hmdForward.v[0] << "," << hmdForward.v[1] << "," << hmdForward.v[2];
				}
				if (gameType == 0 || gameType == 1 || gameType == 2) {
					axisState.x = 0;
					axisState.y = walkTouch;
					if (useContDirForStraf || useContDirForRev) {
						axisState.x = walkTouch * touchX;
						axisState.y = axisState.y * touchY;
					}
					try {
						applyAxisMovement(0, axisState);
					}
					catch (std::exception& e) {
						LOG(INFO) << "Exception caught while applying virtual step movement: " << e.what();
					}
				}
				else if (gameType == 9999) {
					applyClickMovement(0);
				}
				else if (gameType == 3) {
					applyGripMovement(0);
				}
			}
			if (gameType == 7) {
				if (pressedFlag) {
					/*INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x57, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(1, input, sizeof(INPUT));*/

					pressedFlag = false;
				}
			}
			else if (gameType == 8) {
				if (pressedFlag) {
					/*INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x26, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(1, input, sizeof(INPUT));*/
				}
				pressedFlag = false;
			}
		}
	}


	void WalkInPlaceTabController::stopMovement(uint32_t deviceIdOLD) {
		uint32_t deviceId = vrLocoContID;
		if (vrLocoContID == vr::k_unTrackedDeviceIndexInvalid) {
			deviceId = deviceIdOLD;
		}
		if (gameType == 0 || gameType == 1 || gameType == 2) {
			vr::VRControllerAxis_t axisState;
			axisState.x = 0;
			axisState.y = 0;
			try {
				vrwalkinplace::VRWalkInPlace vrwalkinplace;
				vrwalkinplace.connect();
				if (gameType != 1) {
					vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				}
				vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
			}
		}
		else if (gameType == 3) {
			try {
				vrwalkinplace::VRWalkInPlace vrwalkinplace;
				vrwalkinplace.connect();
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Grip, 0.0);
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while applying virtual grip movement: " << e.what();
			}
		}
		else if (gameType == 9999) { //click only disabled atm
			try {
				vrwalkinplace::VRWalkInPlace vrwalkinplace;
				vrwalkinplace.connect();
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual teleport movement: " << e.what();
			}
		}
		else if (gameType == 7) {
			/*INPUT input[2];
			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wVk = 0;
			input[0].ki.wScan = MapVirtualKey(0x57, 0);
			input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
			input[0].ki.time = 0;
			input[0].ki.dwExtraInfo = 0;
			SendInput(2, input, sizeof(INPUT));*/
		}
		else if (gameType == 8) {
			/*INPUT input[2];
			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wVk = 0;
			input[0].ki.wScan = MapVirtualKey(0x26, 0);
			input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
			input[0].ki.time = 0;
			input[0].ki.dwExtraInfo = 0;
			SendInput(2, input, sizeof(INPUT));*/
		}
	}

	void WalkInPlaceTabController::stopClickMovement(uint32_t deviceId) {
		if (gameType == 0) {
			try {
				vrwalkinplace::VRWalkInPlace vrwalkinplace;
				vrwalkinplace.connect();
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				pressedFlag = true;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
			}
		}
	}

	void WalkInPlaceTabController::applyVirtualStep() {
		uint32_t deviceId = vrLocoContID;
		vr::VRControllerAxis_t axisState;
		axisState.x = 0;
		axisState.y = 1;
		applyAxisMovement(deviceId, axisState);
	}

	void WalkInPlaceTabController::applyAxisMovement(uint32_t deviceIdOLD, vr::VRControllerAxis_t axisState) {
		uint32_t deviceId = vrLocoContID;
		if (vrLocoContID == vr::k_unTrackedDeviceIndexInvalid) {
			deviceId = deviceIdOLD;
		}
		try {
			vrwalkinplace::VRWalkInPlace vrwalkinplace;
			vrwalkinplace.connect();
			if (gameType == 0 || gameType == 1 || gameType == 2) {
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
				if (gameType != 1) {
					if (pressedFlag || gameType == 2) {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					else {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
				}
			}
		}
		catch (std::exception& e) {
			//LOG(INFO) << "Exception caught while applying virtual axis movement: " << e.what();
		}
	}

	void WalkInPlaceTabController::applyClickMovement(uint32_t deviceId) {
		if (pressedFlag) {
			try {
				vrwalkinplace::VRWalkInPlace vrwalkinplace;
				vrwalkinplace.connect();
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				pressedFlag = false;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while applying virtual telport movement: " << e.what();
			}
		}
		else {
			try {
				vrwalkinplace::VRWalkInPlace vrwalkinplace;
				vrwalkinplace.connect();
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				pressedFlag = true;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while resetting virtual telport movement: " << e.what();
			}
		}
		unnTouchedCount = 0;
	}

	void WalkInPlaceTabController::applyGripMovement(uint32_t deviceId) {
		try {
			vrwalkinplace::VRWalkInPlace vrwalkinplace;
			vrwalkinplace.connect();
			vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_Grip, 0.0);
		}
		catch (std::exception& e) {
			//LOG(INFO) << "Exception caught while applying virtual telport movement: " << e.what();
		}
		unnTouchedCount = 0;
	}



} // namespace walkinplace
