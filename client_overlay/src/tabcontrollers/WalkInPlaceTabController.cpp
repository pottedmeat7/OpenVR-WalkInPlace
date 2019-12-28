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
		reloadProfiles();
		reloadSettings();
		gameType = std::make_shared <GameType>();
		gameType->inputType = InputType::touchpad;
	}


	void WalkInPlaceTabController::initStage2(OverlayController * parent, QQuickWindow * widget) {
		this->parent = parent;
		this->widget = widget;
	}


	void WalkInPlaceTabController::eventLoopTick() {
		if (!initializedDataModel) {
			if (!dataTrainingRequired) {
				loadDataModel();
			}
		}
		else if (wipEnabled) {
			if (!initializedDriver) {
				if (hasTwoControllers) {
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
				runSampleOnModel();
				/*HKEY hKey;
				LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"\\Software\\Valve\\Steam\\ActiveProcess", 0, KEY_READ, &hKey);
				bool bExistsAndSuccess(lRes == ERROR_SUCCESS);
				bool bDoesNotExistsSpecifically(lRes == ERROR_FILE_NOT_FOUND);
				if (bExistsAndSuccess) {
					std::wstring strValueOfBinDir;
					std::wstring strKeyDefaultValue;
					LONG temp = GetStringRegKey(hKey, L"pid", strValueOfBinDir, L"bad");
					if (temp == ERROR_SUCCESS) {
						LOG(INFO) << "Steam Running App: " << strValueOfBinDir;
					}
				}*/
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
				setDeviceRenderModel(controlSelectOverlayHandle, 0, 1, 1, 1, 1, 1, 1);
			}
		}
		if (!initializedDriver || !wipEnabled) {
			if (hmdID == vr::k_unTrackedDeviceIndexInvalid ||
				(controller1ID == vr::k_unTrackedDeviceIndexInvalid || controller2ID == vr::k_unTrackedDeviceIndexInvalid) ||
				(tracker1ID == vr::k_unTrackedDeviceIndexInvalid || tracker2ID == vr::k_unTrackedDeviceIndexInvalid)) {
				bool newDeviceAdded = false;
				for (uint32_t id = maxValidDeviceId; id < vr::k_unMaxTrackedDeviceCount; id++) {
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
							LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
						}
						maxValidDeviceId = id + 1;
					}
				}
				int cntrlIdx = 0;
				int trkrIdx = 0;
				int cntrlCount = 0;
				for (auto dev : deviceInfos) {
					if (dev->deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
						cntrlCount++;
						if (cntrlCount >= 2) {
							hasTwoControllers = true;
						}
					}
					if (currentProfileIdx >= 0 && currentProfileIdx < walkInPlaceProfiles.size()) {
						if (dev->serial.find("ovrwip") == std::string::npos) {
							auto p = &walkInPlaceProfiles[currentProfileIdx];
							if (controller1ID == vr::k_unTrackedDeviceIndexInvalid && p->cntrl1Idx >= 0) {
								if (dev->deviceClass == p->cntrl1Type && cntrlIdx == p->cntrl1Idx) {
									controller1ID = dev->openvrId;
									dev->isTrackedAsCNTRL = true;
								}
							}
							else if (controller2ID == vr::k_unTrackedDeviceIndexInvalid && p->cntrl2Idx >= 0) {
								if (dev->deviceClass == p->cntrl2Type && cntrlIdx == p->cntrl2Idx) {
									controller2ID = dev->openvrId;
									dev->isTrackedAsCNTRL = true;
								}
							}
							if (tracker1ID == vr::k_unTrackedDeviceIndexInvalid && p->trkr1Idx >= 0) {
								if (dev->deviceClass == p->trkr1Type && trkrIdx == p->trkr1Idx) {
									tracker1ID = dev->openvrId;
									dev->isTrackedAsTRKR = true;
								}
							}
							else if (tracker2ID == vr::k_unTrackedDeviceIndexInvalid && p->trkr2Idx >= 0) {
								if (dev->deviceClass == p->trkr2Type && trkrIdx == p->trkr2Idx) {
									tracker2ID = dev->openvrId;
									dev->isTrackedAsTRKR = true;
								}
							}
							if (dev->deviceClass == p->cntrl1Type || dev->deviceClass == p->cntrl2Type) {
								cntrlIdx++;
							}
							else if (dev->deviceClass == p->trkr1Type || dev->deviceClass == p->trkr2Type) {
								trkrIdx++;
							}
						}
						else {
							ovrwipCNTRLID = dev->openvrId;
						}
					}
					else {
						if (dev->deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) {
							if (dev->serial.find("ovrwip") == std::string::npos) {
								if (controller1ID == vr::k_unTrackedDeviceIndexInvalid) {
									controller1ID = dev->openvrId;
									dev->isTrackedAsCNTRL = true;
								}
								else if (controller2ID == vr::k_unTrackedDeviceIndexInvalid && dev->openvrId != controller1ID) {
									if (!initializedProfile) {
										controller2ID = dev->openvrId;
										dev->isTrackedAsCNTRL = true;
									}
								}
							}
							else {
								ovrwipCNTRLID = dev->openvrId;
							}
						}
						else if (dev->deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker) {
							if (tracker1ID == vr::k_unTrackedDeviceIndexInvalid) {
								tracker1ID = dev->openvrId;
								//dev->isTrackedAsTRKR = true;
							}
							else if (tracker2ID == vr::k_unTrackedDeviceIndexInvalid && dev->openvrId != tracker1ID) {
								tracker2ID = dev->openvrId;
								//dev->isTrackedAsTRKR = true;
							}
						}
					}
				}
			}
		}
	}

	LONG WalkInPlaceTabController::GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
	{
		strValue = strDefaultValue;
		WCHAR szBuffer[512];
		DWORD dwBufferSize = sizeof(szBuffer);
		ULONG nError;
		nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
		if (ERROR_SUCCESS == nError)
		{
			strValue = szBuffer;
		}
		return nError;
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

	bool WalkInPlaceTabController::isWIPEnabled() {
		return wipEnabled;
	}

	double WalkInPlaceTabController::getStepTime() {
		return 0;
	}

	int WalkInPlaceTabController::getGameType() {
		if (gameType->inputType == InputType::touchpad) {
			if (gameType->alwaysHeld) {
				return 2;
			}
			else if (gameType->useClick) {
				return 0;
			}
			else {
				return 1;
			}
		}
		else if (gameType->inputType == InputType::joystick) {
			if (gameType->alwaysHeld) {
				return 5;
			}
			else if (gameType->useClick) {
				return 3;
			}
			else {
				return 4;
			}
		}
		else if (gameType->inputType == InputType::grip) {
			return 6;
		}
		else if (gameType->inputType == InputType::keyWASD) {
			return 7;
		}
		else {
			return 8;
		}
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
		return trackHMDVel;
	}

	bool WalkInPlaceTabController::getTrackHMDRot() {
		return trackHMDRot;
	}

	float WalkInPlaceTabController::getMinTouch() {
		return minTouch;
	}

	float WalkInPlaceTabController::getMidTouch() {
		return midTouch;
	}

	float WalkInPlaceTabController::getMaxTouch() {
		return maxTouch;
	}

	void WalkInPlaceTabController::clearSamplesAndModel() {
		try {
			dataModel.clear();
			dataModel.insert_rows(0, 1);
			dataModel.insert_cols(0, TOUCH_VAL_IDX + 1);
			sample.clear();
			sample.insert_rows(0, 1);
			sample.insert_cols(0, TOUCH_VAL_IDX - 0);
			sampleResults.clear();
			sampleResults.insert_rows(0, 1);
		}
		catch (std::exception& e) {
			LOG(INFO) << "Exception caught while initializing sample and model: " << e.what();
		}
	}

	void WalkInPlaceTabController::loadDataModel() {
		try {
			std::string model_loc = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + model_file_name + ".xml";
			if (!boost::filesystem::exists(model_loc)) {
				dataModel = dataModel.t();
				bool loaded = dataModel.load(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + "/" + model_file_name + model_file_type);
				if (loaded) {
					initializedDataModel = true;
					dataTrainingRequired = false;
					arma::mat known_input = dataModel.submat(0, 0, TOUCH_VAL_IDX - 1, dataModel.n_cols - 1);
					arma::rowvec known_responses = dataModel.row(TOUCH_VAL_IDX);
					mlpack::data::MinMaxScaler scale(0,5);
					arma::mat known_scaled;
					scale.Fit(known_input);
					scale.Transform(known_input, known_scaled);
					vrLocoRegress = mlpack::regression::LinearRegression(known_scaled, known_responses);// , weights);
					//std::ofstream file1{ model_file_name + "_hmd.xml" };
					//boost::archive::xml_oarchive tr_lr_hmd{ file1 };
					//tr_lr_hmd << boost::serialization::make_nvp("vrRegress", vrLocoRegress);
					/*known_input.clear();
					known_scaled.clear();
					known_responses.clear();
					known_input = arma::abs(dataModel.submat(CNTRL1_X_VEL_IDX, 0, TOUCH_VAL_IDX - 3, dataModel.n_cols - 1));
					known_responses = dataModel.row(TOUCH_VAL_IDX);
					scale.Fit(known_input);
					scale.Transform(known_input, known_scaled);
					vrLocoRegressCNTRL = mlpack::regression::LinearRegression(known_scaled, known_responses);// , weights);*/
					//std::ofstream file2{ model_file_name + "_cntrl.xml" };
					//boost::archive::xml_oarchive tr_lr_cntrl{ file2 };
					//tr_lr_cntrl << boost::serialization::make_nvp("vrRegressCNTRL", vrLocoRegressCNTRL);
				}
				else {
					dataTrainingRequired = true;
					clearSamplesAndModel();
					LOG(INFO) << "unable to load data model at: " << QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + "/" + model_file_name + model_file_type;
				}
			}
			else {
				mlpack::data::Load(model_loc, model_file_name + "_hmd", vrLocoRegress);
				//mlpack::data::Load(model_loc, model_file_name + "_cntrl", vrLocoRegressCNTRL);
				//vrLocoRegress = mlpack::regression::LinearRegression(known_scaled, known_responses);// , weights);
			}
		}
		catch (std::exception& e) {
			dataModel = dataModel.t();
			initializedDataModel = false;
			dataTrainingRequired = true;
			LOG(INFO) << "Exception caught while loading data model: " << e.what();
		}
	}

	QList<qreal> WalkInPlaceTabController::getModelData() {
		bool loaded = initializedDataModel;
		if (!initializedDataModel) {
			loadDataModel();
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
					vals.push_back((float)dataModel(TOUCH_VAL_IDX, i));
				}
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while getting the data model: " << e.what();
			}
		}
		return vals;
	}

	void WalkInPlaceTabController::completeTraining() {
		try {
			//dataModel = dataModel.t();
			dataModel.shed_row(0);
			mlpack::data::Save(QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + "/" + model_file_name + model_file_type, dataModel);
			clearSamplesAndModel();
			initializedDataModel = false;
			dataTrainingRequired = false;
		}
		catch (std::exception& e) {
			LOG(INFO) << "Exception caught while saving data model: " << e.what();
		}
	}

	QList<qreal> WalkInPlaceTabController::getHMDSample() {
		QList<qreal> vals;
		if (wipEnabled) {
			try {
				vals.push_back(validSample ? 1 : 0);
				vals.push_back(lastValidSampleMKi);
				for (int i = 0; i < sample.n_rows; i++) {
					vals.push_back((float)sample(i, HMD_Y_VEL_IDX));
					//vals.push_back((float)hmdSample(1, i));
					//vals.push_back((float)hmdSample(2, i));
				}
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while getting the hmd sample: " << e.what();
			}
		}
		return vals;
	}

	QList<qreal> WalkInPlaceTabController::getCNTRLSample() {
		QList<qreal> vals;
		if (wipEnabled) {
			try {
				vals.push_back(validSample ? 1 : 0);
				vals.push_back(lastValidSampleMKi);
				for (int i = 0; i < sample.n_rows; i++) {
					//vals.push_back((float)sample(i, CNTRL1_Y_VEL_IDX));
					//vals.push_back((float)sample(i, CNTRL2_Y_VEL_IDX));
				}
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while getting the controller sample: " << e.what();
			}
		}

		return vals;
	}

	QList<qreal> WalkInPlaceTabController::getTRKRSample() {
		QList<qreal> vals;
		if (wipEnabled) {
			try {
				vals.push_back(validSample ? 1 : 0);
				vals.push_back(lastValidSampleMKi);
				for (int i = 0; i < sample.n_rows; i++) {
					//vals.push_back((float)sample(i, TRKR1_Y_VEL_IDX));
					//vals.push_back((float)sample(i, TRKR2_Y_VEL_IDX));
				}
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while getting the tracker sample: " << e.what();
			}
		}
		return vals;
	}

	QList<qreal> WalkInPlaceTabController::trainingDataSample(float scaleSpeed, double tdiff) {
		initializedDataModel = false;
		dataTrainingRequired = true;
		vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
		bool firstController = true;
		bool firstTracker = true;
		vr::HmdVector3d_t hmdVel = { 0, 0, 0 };
		vr::HmdVector3d_t hmdAcc = { 0, 0, 0 };
		vr::HmdVector3d_t hmdRotVel = { 0, 0, 0 };
		vr::HmdVector3d_t cont1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t cont2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker2Vel = { 0, 0, 0 };
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

		if (dataModel.n_rows > 1) {
			hmdAcc.v[0] = (hmdVel.v[0] - lastHmdVel.v[0]);// / tdiff;
			hmdAcc.v[1] = (hmdVel.v[1] - lastHmdVel.v[1]);// / tdiff;
			hmdAcc.v[2] = (hmdVel.v[2] - lastHmdVel.v[2]);// / tdiff;

			hmdRotVel.v[0] = (devYaw - lastHmdRot.v[0]) / tdiff;
			hmdRotVel.v[1] = (devPitch - lastHmdRot.v[1]) / tdiff;
			hmdRotVel.v[2] = (devRoll - lastHmdRot.v[2]) / tdiff;
		}

		lastHmdVel.v[0] = hmdVel.v[0];
		lastHmdVel.v[1] = hmdVel.v[1];
		lastHmdVel.v[2] = hmdVel.v[2];

		lastHmdRot.v[0] = devYaw;
		lastHmdRot.v[1] = devPitch;
		lastHmdRot.v[2] = devRoll;

		if (controller1ID != vr::k_unTrackedDeviceIndexInvalid) {
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

			if (controller2ID != vr::k_unTrackedDeviceIndexInvalid) {
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
			}
		}

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
			}
		}
		try {
			int n = dataModel.n_rows;
			dataModel.insert_rows(n, 1);

			dataModel(n, HMD_X_VEL_IDX) = hmdVel.v[0];
			dataModel(n, HMD_Y_VEL_IDX) = hmdVel.v[1];
			dataModel(n, HMD_Z_VEL_IDX) = hmdVel.v[2];
			dataModel(n, HMD_X_ACC_IDX) = hmdVel.v[0];
			dataModel(n, HMD_Y_ACC_IDX) = hmdVel.v[1];
			dataModel(n, HMD_Z_ACC_IDX) = hmdVel.v[2];
			dataModel(n, HMD_YAW_VEL_IDX) = hmdRotVel.v[0]; // yaw rotational velocity
			dataModel(n, HMD_PITCH_VEL_IDX) = hmdRotVel.v[1]; // pitch rotational velocity
			/*dataModel(n, CNTRL1_Y_VEL_IDX) = std::abs(cont1Vel.v[1]);
			dataModel(n, CNTRL2_Y_VEL_IDX) = std::abs(cont2Vel.v[1]);
			dataModel(n, CNTRL1_X_VEL_IDX) = std::abs(cont1Vel.v[0]);
			dataModel(n, CNTRL2_X_VEL_IDX) = std::abs(cont2Vel.v[0]);
			dataModel(n, CNTRL1_Z_VEL_IDX) = std::abs(cont1Vel.v[2]);
			dataModel(n, CNTRL2_Z_VEL_IDX) = std::abs(cont2Vel.v[2]);
			dataModel(n, TRKR1_Y_VEL_IDX) = tracker1Vel.v[1];
			dataModel(n, TRKR2_Y_VEL_IDX) = tracker2Vel.v[1];*/
			dataModel(n, TOUCH_VAL_IDX) = scaleSpeed;
		}
		catch (std::exception& e) {
			LOG(INFO) << "Exception caught while building data model: " << e.what();
		}

		QList<qreal> vals;
		vals.push_back(hmdAcc.v[1]);// hmdRotVel.v[0]);// hmdVel.v[0]); // yaw vel
		vals.push_back(hmdVel.v[1]);
		vals.push_back(hmdAcc.v[0]);// hmdRotVel.v[1]);//hmdVel.v[2]); // pitch vel

		vals.push_back(cont1Vel.v[1]);
		vals.push_back(cont2Vel.v[1]);

		vals.push_back(tracker1Vel.v[1]);
		vals.push_back(tracker2Vel.v[1]);
		//LOG(INFO) << "HMD VALS: " << hmdVel.v[0] << "," << hmdVel.v[1] << "," << hmdVel.v[2];	

		return vals;
	}

	QList<qreal> WalkInPlaceTabController::getGraphPoses(double tdiff) {
		vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);
		auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		bool firstController = true;
		bool firstTracker = true;
		vr::HmdVector3d_t hmdVel = { 0, 0, 0 };
		vr::HmdVector3d_t hmdRotVel = { 0, 0, 0 };
		vr::HmdVector3d_t cont1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t cont2Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker1Vel = { 0, 0, 0 };
		vr::HmdVector3d_t tracker2Vel = { 0, 0, 0 };
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

		lastHmdRot.v[0] = devYaw;
		lastHmdRot.v[1] = devPitch;
		lastHmdRot.v[2] = devRoll;

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

		if (controller2ID != vr::k_unTrackedDeviceIndexInvalid) {
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
		}

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
			}
		}

		QList<qreal> vals;
		vals.push_back(hmdRotVel.v[0]);//hmdVel.v[0]); // yaw vel
		vals.push_back(hmdVel.v[1]);
		vals.push_back(hmdRotVel.v[1]);//hmdVel.v[2]); // pitch vel

		vals.push_back(cont1Vel.v[1]);
		vals.push_back(cont2Vel.v[1]);

		vals.push_back(tracker1Vel.v[1]);
		vals.push_back(tracker2Vel.v[1]);

		return vals;

	}

	bool WalkInPlaceTabController::isStepDetected() {
		return validSample;
	}

	QList<QString> WalkInPlaceTabController::getDataModelNames() {
		QList<QString> models;
		std::string model_loc = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString();
		try {
			boost::filesystem::directory_iterator end_iter;
			for (boost::filesystem::directory_iterator dir_itr(model_loc); dir_itr != end_iter; ++dir_itr) {
				try {
					if (boost::filesystem::is_regular_file(dir_itr->status())) {
						if (dir_itr->path().extension().generic_string().find("csv") != std::string::npos) {
							models.push_back(QString::fromStdString(dir_itr->path().filename().generic_string()));
						}
					}
				}
				catch (const std::exception & ex) {
				}
			}
		}
		catch (const std::exception & ex) {
			LOG(INFO) << "Error while getting data model names: " << ex.what();
		}
		return models;
	}

	void WalkInPlaceTabController::addDataModel(QString name) {
		model_file_name = name.toStdString();
		clearSamplesAndModel();
	}

	void WalkInPlaceTabController::applyDataModel(QString name) {
		model_file_name = name.toStdString();
		clearSamplesAndModel();
		initializedDataModel = false;
		dataTrainingRequired = false;
	}

	void WalkInPlaceTabController::deleteDataModel(QString name) {
		std::string model_loc = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absolutePath().toStdString() + "/" + name.toStdString() + model_file_type;
		if (std::remove(model_loc.c_str()) != 0) {
			LOG(INFO) << "could not delete, you can delete manually from: " << model_loc;
		}
		clearSamplesAndModel();
	}

	void WalkInPlaceTabController::reloadSettings() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		settings->endGroup();
	}

	void WalkInPlaceTabController::reloadProfiles() {
		walkInPlaceProfiles.clear();
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		auto profileCount = settings->beginReadArray("walkInPlaceProfiles");
		for (int i = 0; i < profileCount; i++) {
			settings->setArrayIndex(i);
			walkInPlaceProfiles.emplace_back();
			auto& entry = walkInPlaceProfiles[i];
			entry.profileName = settings->value("profileName").toString().toStdString();
			entry.modelFile = settings->value("modelFile", QString((model_file_name).c_str())).toString().toStdString();
			entry.wipEnabled = settings->value("wipEnabled", false).toBool();
			entry.gameType = settings->value("gameType", 0).toInt();
			entry.hmdType = settings->value("hmdType", 0).toInt();
			entry.buttonControlSelect = settings->value("buttonControlSelect", 0).toInt();
			entry.buttonEnables = settings->value("buttonEnables", false).toBool();
			entry.useTrackers = settings->value("useTrackers", false).toBool();
			entry.trackHMDVel = settings->value("trackHMDVel", true).toBool();
			entry.trackHMDRot = settings->value("trackHMDRot", true).toBool();
			entry.disableButton = settings->value("disableButton", 0).toInt();
			entry.minTouch = settings->value("minTouch", 0.25).toFloat();
			entry.midTouch = settings->value("midTouch", 0.83).toFloat();
			entry.maxTouch = settings->value("maxTouch", 1).toFloat();
			entry.cntrl1Idx = settings->value("cntrl1Idx", 0).toInt();
			entry.cntrl2Idx = settings->value("cntrl2Idx", -1).toInt();
			entry.trkr1Idx = settings->value("trkr1Idx", -1).toInt();
			entry.trkr2Idx = settings->value("trkr2Idx", -1).toInt();
			entry.cntrl1Type = settings->value("cntrl1Type", vr::TrackedDeviceClass::TrackedDeviceClass_Controller).toInt();
			entry.cntrl2Type = settings->value("cntrl2Type", vr::TrackedDeviceClass::TrackedDeviceClass_Controller).toInt();
			entry.trkr1Type = settings->value("trkr1Type", vr::TrackedDeviceClass::TrackedDeviceClass_GenericTracker).toInt();
			entry.trkr2Type = settings->value("trkr2Type", vr::TrackedDeviceClass::TrackedDeviceClass_GenericTracker).toInt();
		}
		settings->endArray();
		settings->endGroup();
	}

	void WalkInPlaceTabController::saveSettings() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		settings->endGroup();
		settings->sync();
	}


	void WalkInPlaceTabController::saveProfiles() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("walkInPlaceSettings");
		settings->beginWriteArray("walkInPlaceProfiles");
		unsigned i = 0;
		for (auto& p : walkInPlaceProfiles) {
			settings->setArrayIndex(i);
			settings->setValue("profileName", QString::fromStdString(p.profileName));
			settings->setValue("modelFile", QString::fromStdString(p.modelFile));
			settings->setValue("wipEnabled", p.wipEnabled);
			settings->setValue("gameType", p.gameType);
			settings->setValue("hmdType", p.hmdType);
			settings->setValue("buttonControlSelect", p.buttonControlSelect);
			settings->setValue("useTrackers", p.useTrackers);
			settings->setValue("trackHMDVel", p.trackHMDVel);
			settings->setValue("trackHMDRot", p.trackHMDRot);
			settings->setValue("disableButton", p.disableButton);
			settings->setValue("minTouch", p.minTouch);
			settings->setValue("midTouch", p.midTouch);
			settings->setValue("maxTouch", p.maxTouch);
			settings->setValue("buttonEnables", p.buttonEnables);
			settings->setValue("cntrl1Idx", p.cntrl1Idx);
			settings->setValue("cntrl2Idx", p.cntrl2Idx);
			settings->setValue("trkr1Idx", p.trkr1Idx);
			settings->setValue("trkr2Idx", p.trkr2Idx);
			settings->setValue("cntrl1Type", p.cntrl1Type);
			settings->setValue("cntrl2Type", p.cntrl2Type);
			settings->setValue("trkr1Type", p.trkr1Type);
			settings->setValue("trkr2Type", p.trkr2Type);
			i++;
		}
		settings->endArray();
		settings->endGroup();
		settings->sync();
	}

	unsigned WalkInPlaceTabController::getProfileCount() {
		return (unsigned)walkInPlaceProfiles.size();
	}

	QString WalkInPlaceTabController::getProfileName(unsigned index) {
		if (index >= walkInPlaceProfiles.size()) {
			return QString();
		}
		else {
			return QString::fromStdString(walkInPlaceProfiles[index].profileName);
		}
	}

	void WalkInPlaceTabController::addProfile(QString name) {
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
		profile->wipEnabled = isWIPEnabled();
		profile->modelFile = (model_file_name);
		profile->gameType = getGameType();
		profile->hmdType = hmdType;
		profile->buttonControlSelect = buttonControlSelect;
		profile->useTrackers = useTrackers || !trackHMDVel;
		profile->trackHMDVel = trackHMDVel;
		profile->trackHMDRot = trackHMDRot;
		profile->disableButton = disableButton;
		profile->minTouch = minTouch;
		profile->midTouch = midTouch;
		profile->maxTouch = maxTouch;
		profile->buttonEnables = buttonEnables;
		int cntrlIdx = 0;
		int trkrIdx = -1;
		bool cntrl1Saved = false;
		bool trkr1Saved = false;
		for (int i = 0; i < 4; i++) {
			if (getDeviceEnabled(2, i, 0)) {
				if (!cntrl1Saved) {
					profile->cntrl1Idx = i;
					profile->cntrl1Type = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;
					cntrl1Saved = true;
				}
				else {
					profile->cntrl2Idx = i;
					profile->cntrl2Type = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;
				}
			}
			if (getDeviceEnabled(2, i, 1)) {
				if (!trkr1Saved) {
					profile->trkr1Idx = i;
					profile->trkr1Type = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;
					trkr1Saved = true;
				}
				else {
					profile->trkr2Idx = i;
					profile->trkr2Type = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;
				}
			}
			if (getDeviceEnabled(3, i, 0)) {
				if (!cntrl1Saved) {
					profile->cntrl1Idx = i;
					profile->cntrl1Type = vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker;
					cntrl1Saved = true;
				}
				else {
					profile->cntrl2Idx = i;
					profile->cntrl2Type = vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker;
				}
			}
			if (getDeviceEnabled(3, i, 1)) {
				if (!trkr1Saved) {
					profile->trkr1Idx = i;
					profile->trkr1Type = vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker;
					trkr1Saved = true;
				}
				else {
					profile->trkr2Idx = i;
					profile->trkr2Type = vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker;
				}
			}
		}
		saveProfiles();
		OverlayController::appSettings()->sync();
	}

	void WalkInPlaceTabController::applyProfile(unsigned index) {
		if (index < walkInPlaceProfiles.size()) {
			currentProfileIdx = index;
			auto& profile = walkInPlaceProfiles[index];
			hmdType = profile.hmdType;
			buttonControlSelect = profile.buttonControlSelect;
			useTrackers = profile.useTrackers || !profile.trackHMDVel;
			trackHMDVel = profile.trackHMDVel;
			disableButton = profile.disableButton;
			minTouch = profile.minTouch;
			midTouch = profile.midTouch;
			maxTouch = profile.maxTouch;
			buttonEnables = profile.buttonEnables;

			enableWIP(profile.wipEnabled);
			setGameType(profile.gameType);
			setHMDType(profile.hmdType);
			setButtonControlSelect(profile.buttonControlSelect);
			setTrackHMDVel(profile.trackHMDVel);
			setTrackHMDRot(profile.trackHMDRot);
			setDisableButton(profile.disableButton);
			setButtonEnables(profile.buttonEnables);
			setMinTouch(profile.minTouch);
			setMidTouch(profile.midTouch);
			setMaxTouch(profile.maxTouch);

			enableDevice(2, 0, false, 0);
			enableDevice(2, 1, false, 0);
			enableDevice(2, 2, false, 0);
			enableDevice(2, 3, false, 0);
			enableDevice(3, 0, false, 0);
			enableDevice(3, 1, false, 0);
			enableDevice(3, 2, false, 0);
			enableDevice(3, 3, false, 0);
			enableDevice(2, 0, false, 1);
			enableDevice(2, 1, false, 1);
			enableDevice(2, 3, false, 1);
			enableDevice(2, 4, false, 1);
			enableDevice(3, 0, false, 1);
			enableDevice(3, 1, false, 1);
			enableDevice(3, 3, false, 1);
			enableDevice(3, 4, false, 1);
			if (profile.cntrl1Idx >= 0) {
				controller1ID = vr::k_unTrackedDeviceIndexInvalid;
				enableDevice(profile.cntrl1Type, profile.cntrl1Idx, true, 0);
				if (profile.cntrl2Idx >= 0) {
					controller2ID = vr::k_unTrackedDeviceIndexInvalid;
					enableDevice(profile.cntrl2Type, profile.cntrl2Idx, true, 0);
				}
				else {
					controller2ID = vr::k_unTrackedDeviceIndexInvalid;
				}
			}
			else {
				controller1ID = vr::k_unTrackedDeviceIndexInvalid;
				//enableDevice(profile.cntrl1Type, profile.cntrl1Idx, false, 0);
			}
			if (profile.trkr1Idx >= 0) {
				tracker1ID = vr::k_unTrackedDeviceIndexInvalid;
				enableDevice(profile.trkr1Type, profile.trkr1Idx, true, 1);
				if (profile.trkr2Idx >= 0) {
					tracker2ID = vr::k_unTrackedDeviceIndexInvalid;
					enableDevice(profile.trkr2Type, profile.trkr2Idx, true, 1);
				}
				else {
					tracker2ID = vr::k_unTrackedDeviceIndexInvalid;
				}
			}
			else {
				tracker1ID = vr::k_unTrackedDeviceIndexInvalid;
				tracker2ID = vr::k_unTrackedDeviceIndexInvalid;
			}

			initializedProfile = true;

			auto settings = OverlayController::appSettings();
			settings->beginGroup("walkInPlaceSettings");
			auto profileCount = settings->beginReadArray("walkInPlaceProfiles");
			for (int i = 0; i < profileCount; i++) {
				settings->setArrayIndex(i);
				if (index == i) {
					maxSNHMD = settings->value("maxHMDSampleSize", 16).toInt();
					startSNHMD = settings->value("startHMDSampleSize", 12).toInt();
					reqSNHMD = settings->value("ongoingHMDSampleSize", 4).toInt();
					maxSNTRKR = settings->value("maxTRKRSampleSize", 19).toInt();
					startSNTRKR = settings->value("startTRKRSampleSize", 14).toInt();
					reqSNTRKR = settings->value("ongoingTRKRSampleSize", 8).toInt();
					maxSNCNTRL = settings->value("maxCNTRLSampleSize", 12).toInt();
					reqSNCNTRL = settings->value("ongoingCNTRLSampleSize", 10).toInt();
					hmdVelVariance = settings->value("hmdVelVariance", 0.07).toFloat();
					hmdMinDVPerSN = settings->value("hmdAccelPercentAccurate", 0.75).toFloat();
					trkrVariance = settings->value("trkrVelVariance", 0.07).toFloat();
					cntrlVariance = settings->value("cntrlVelVariance", 0.07).toFloat();
				}
			}
			settings->endArray();
			settings->endGroup();

			applyDataModel(QString::fromStdString(profile.modelFile));
		}
	}

	void WalkInPlaceTabController::deleteProfile(unsigned index) {
		if (index < walkInPlaceProfiles.size()) {
			initializedProfile = true;
			auto pos = walkInPlaceProfiles.begin() + index;
			walkInPlaceProfiles.erase(pos);
			saveProfiles();
			OverlayController::appSettings()->sync();
		}
	}

	void WalkInPlaceTabController::enableWIP(bool enable) {
		wipEnabled = enable;
		if (!enable && initializedDriver) {
			stopMovement();
		}
	}

	void WalkInPlaceTabController::setMinTouch(float value) {
		minTouch = value;
	}

	void WalkInPlaceTabController::setMidTouch(float value) {
		midTouch = value;
	}

	void WalkInPlaceTabController::setMaxTouch(float value) {
		maxTouch = value;
	}

	void WalkInPlaceTabController::setTrackHMDVel(bool value) {
		trackHMDVel = value;
		if (!trackHMDVel && !useTrackers) {
			useTrackers = true;
		}
	}

	void WalkInPlaceTabController::setTrackHMDRot(bool value) {
		trackHMDRot = value;
	}

	void WalkInPlaceTabController::setHMDMaxVari(float value) {
		hmdVelVariance = value;
	}

	void WalkInPlaceTabController::setDisableButton(int buttonId) {
		disableButton = buttonId;
	}

	bool WalkInPlaceTabController::getDeviceEnabled(int devClass, int devIdx, int mode) {
		int classIdx = 0;
		for (auto d : deviceInfos) {
			if (d->deviceClass == devClass) {
				if (classIdx == devIdx) {
					if (mode == 0) {
						return d->isTrackedAsCNTRL;
					}
					else if (mode == 1) {
						return d->isTrackedAsTRKR;
					}
				}
				classIdx++;
			}
		}
		/*if (currentProfileIdx >= 0 && currentProfileIdx < walkInPlaceProfiles.size()) {
			WalkInPlaceProfile* profile = &walkInPlaceProfiles[currentProfileIdx];
			if (mode == 0 && ((profile->cntrl1Idx < 0 && devIdx == 0) || (profile->cntrl2Idx < 0 && devIdx == 1))) {
				return true;
			}
			else if ((mode == 0 && ((devIdx == profile->cntrl1Idx && devClass == profile->cntrl1Type)
				|| (devIdx == profile->cntrl2Idx && devClass == profile->cntrl2Type)))
				|| (mode == 1 && ((devIdx == profile->trkr1Idx && devClass == profile->trkr1Type)
					|| (devIdx == profile->trkr2Idx && devClass == profile->trkr2Type)))) {
				return true;
			}
		}
		else if ((mode == 0 && (devIdx == 0 || devIdx == 1))) {
			return true;
		}*/
		return false;
	}

	void WalkInPlaceTabController::setDirectionDevice(int choice) {
		//["None, Use Game Default","1st controller", "2nd controller", "1st tracker", "2nd tracker", "3rd controller", "4th controller", "3rd tracker", "4th tracker"]
		switch (choice) {
		case 0:
		case 1:
			directionDevice = vr::k_unTrackedDeviceIndexInvalid;
			break;
		case 2:
		case 6:
			directionDevice = controller1ID;
			break;
		case 3:
		case 7:
			directionDevice = controller2ID;
			break;
		case 4:
		case 8:
			directionDevice = tracker1ID;
			break;
		case 5:
		case 9:
			directionDevice = tracker2ID;
			break;
		default:
			break;
		}
	}

	void WalkInPlaceTabController::enableDevice(int deviceClass, int devIdx, bool enable, int mode) {
		int deviceClassI = 0;
		int idx = 0;
		int foundDevIdx = -1;
		for (auto dev : deviceInfos) {
			if (dev->deviceClass == deviceClass) {
				if (deviceClassI == devIdx) {
					if (mode == 0) {
						if (enable) {
							if (controller1ID == vr::k_unTrackedDeviceIndexInvalid) {
								controller1ID = dev->openvrId;
								dev->isTrackedAsCNTRL = true;
								foundDevIdx = idx;
							}
							else if (controller2ID == vr::k_unTrackedDeviceIndexInvalid) {
								controller2ID = dev->openvrId;
								dev->isTrackedAsCNTRL = true;
								foundDevIdx = idx;
							}
						}
						else if (!enable) {
							if (controller1ID == dev->openvrId) {
								controller1ID = vr::k_unTrackedDeviceIndexInvalid;
								dev->isTrackedAsCNTRL = false;
								foundDevIdx = idx;
							}
							if (controller2ID == dev->openvrId) {
								controller2ID = vr::k_unTrackedDeviceIndexInvalid;
								dev->isTrackedAsCNTRL = false;
								initializedProfile = true;
								foundDevIdx = idx;
							}
						}
					}
					else {
						if (enable) {
							if (tracker1ID == vr::k_unTrackedDeviceIndexInvalid) {
								tracker1ID = dev->openvrId;
								dev->isTrackedAsTRKR = true;
								foundDevIdx = idx;
							}
							else if (tracker2ID == vr::k_unTrackedDeviceIndexInvalid) {
								tracker2ID = dev->openvrId;
								dev->isTrackedAsTRKR = true;
								foundDevIdx = idx;
							}
						}
						else if (!enable) {
							if (tracker1ID == dev->openvrId) {
								tracker1ID = vr::k_unTrackedDeviceIndexInvalid;
								dev->isTrackedAsTRKR = false;
								foundDevIdx = idx;
								if (tracker2ID != vr::k_unTrackedDeviceIndexInvalid) {
									tracker1ID = tracker2ID;
									dev->isTrackedAsTRKR = true;
									tracker2ID = vr::k_unTrackedDeviceIndexInvalid;
								}
							}
							if (tracker2ID == dev->openvrId) {
								tracker2ID = vr::k_unTrackedDeviceIndexInvalid;
								dev->isTrackedAsTRKR = false;
								foundDevIdx = idx;
							}
						}
					}
				}
				deviceClassI++;
			}
			idx++;
		}
		if (tracker1ID == vr::k_unTrackedDeviceIndexInvalid && tracker2ID == vr::k_unTrackedDeviceIndexInvalid) {
			useTrackers = false;
		}
		else {
			useTrackers = true;
			initializedProfile = true;
		}
		if (foundDevIdx >= 0) {
			identifyControlTimerSet = true;
			identifyControlLastTime = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			controlSelectOverlayHandle = foundDevIdx;
			if (enable) {
				setDeviceRenderModel(foundDevIdx, vive_controller_model_index, 0, 1, 0, 1.1, 1.1, 1.1);
			}
			else {
				setDeviceRenderModel(foundDevIdx, vive_controller_model_index, 1, 0, 0, 1.1, 1.1, 1.1);
			}
		}
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

	void WalkInPlaceTabController::setButtonEnables(bool val) {
		buttonEnables = val;
	}

	void WalkInPlaceTabController::setGameType(int type) {
		if (type < 6) {
			gameType->useAxis = true;
			gameType->useClick = !(type == 1 || type == 4);
			gameType->alwaysHeld = (type == 2 || type == 5);
			gameType->inputType = (type < 3) ? InputType::touchpad : InputType::joystick;
		}
		else {
			gameType->useAxis = false;
			gameType->useClick = true;
			gameType->alwaysHeld = false;
			if (type == 6) {
				gameType->inputType = InputType::grip;
			}
			else if (type == 7) {
				gameType->inputType = InputType::keyWASD;
			}
			else if (type == 8) {
				gameType->inputType = InputType::keyArrow;
			}
		}
	}

	void WalkInPlaceTabController::setHMDType(int type) {
		hmdType = type;
	}

	void WalkInPlaceTabController::setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex, float r, float g, float b, float sx, float sy, float sz) {
		if (deviceIndex < deviceInfos.size()) {
			try {
				if (renderModelIndex == 0) {
					for (auto dev : deviceInfos) {
						if (dev->renderModelOverlay != vr::k_ulOverlayHandleInvalid) {
							vr::VROverlay()->DestroyOverlay(dev->renderModelOverlay);
							dev->renderModelOverlay = vr::k_ulOverlayHandleInvalid;
						}
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
				setDeviceRenderModel(controlSelectOverlayHandle, vive_controller_model_index, 1, 1, 0, 1.1, 1.1, 1.1);
			}
		}
	}

	void WalkInPlaceTabController::runSampleOnModel() {
		if (controller1ID != vr::k_unTrackedDeviceIndexInvalid) {
			auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			double tdiff = ((double)(now - timeLastTick));
			bool hmdStep = false;
			inputStateChanged = false;
			//LOG(INFO) << "DT: " << tdiff;
			if (tdiff >= dT) {
				timeLastTick = now;
				if (disableButton == 0 || disableButton == 1) {
					holdingButton = false;
					updateButtonState(controller1ID, true);
					if (controller2ID != vr::k_unTrackedDeviceIndexInvalid) {
						updateButtonState(controller2ID, false);
					}
				}
				if (buttonStatus()) {
					vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, latestDevicePoses, vr::k_unMaxTrackedDeviceCount);

					/*vr::VRInput()->GetSkeletalBoneData(controller1ID, vr::VRSkeletalTransformSpace_Parent, vr::VRSkeletalMotionRange_WithController,
						latestBoneTransforms, vr::k_unMaxTrackedDeviceCount);
					int i = controller1ID;
					LOG(INFO) << "abs bone O: " << latestBoneTransforms[i].orientation.x << "," << latestBoneTransforms[i].orientation.y << "," << latestBoneTransforms[i].orientation.z << "," << latestBoneTransforms[i].orientation.w;
					LOG(INFO) << "abs bone P: " << latestBoneTransforms[i].position.v[0] << "," << latestBoneTransforms[i].position.v[1] << "," << latestBoneTransforms[i].position.v[2] << "," << latestBoneTransforms[i].position.v[3];
					vr::VRInput()->GetSkeletalReferenceTransforms(controller1ID, vr::VRSkeletalTransformSpace_Parent, vr::EVRSkeletalReferencePose::VRSkeletalReferencePose_Fist,
						latestBoneTransforms, vr::k_unMaxTrackedDeviceCount);
					LOG(INFO) << "ref bone O: " << latestBoneTransforms[i].orientation.x << "," << latestBoneTransforms[i].orientation.y << "," << latestBoneTransforms[i].orientation.z << "," << latestBoneTransforms[i].orientation.w;
					LOG(INFO) << "ref bone P: " << latestBoneTransforms[i].position.v[0] << "," << latestBoneTransforms[i].position.v[1] << "," << latestBoneTransforms[i].position.v[2] << "," << latestBoneTransforms[i].position.v[3];
					*/
					vr::HmdVector3d_t hmdVel = { 0, 0, 0 };
					vr::HmdVector3d_t hmdAcc = { 0, 0, 0 };
					vr::HmdVector3d_t hmdRotVel = { 0, 0, 0 };
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

					hmdAcc.v[0] = (hmdVel.v[0] - lastHmdVel.v[0]);// / tdiff;
					hmdAcc.v[1] = (hmdVel.v[1] - lastHmdVel.v[1]);// / tdiff;
					hmdAcc.v[2] = (hmdVel.v[2] - lastHmdVel.v[2]);// / tdiff;

					lastHmdVel.v[0] = hmdVel.v[0];
					lastHmdVel.v[1] = hmdVel.v[1];
					lastHmdVel.v[2] = hmdVel.v[2];

					double devYaw = (180 * std::asin(devForward.v[0])) / M_PI;
					double devPitch = (180 * std::asin(devForward.v[1])) / M_PI;
					double devRoll = (180 * std::asin(devRight.v[1])) / M_PI;

					hmdRotVel.v[0] = (devYaw - lastHmdRot.v[0]) / tdiff;
					hmdRotVel.v[1] = (devPitch - lastHmdRot.v[1]) / tdiff;
					hmdRotVel.v[2] = (devRoll - lastHmdRot.v[2]) / tdiff;

					lastHmdRot.v[0] = devYaw;
					lastHmdRot.v[1] = devPitch;
					lastHmdRot.v[2] = devRoll;

					vr::HmdVector3d_t tracker1Vel = { 0, 0, 0 };
					vr::HmdVector3d_t tracker2Vel = { 0, 0, 0 };
					if (useTrackers && tracker1ID != vr::k_unTrackedDeviceIndexInvalid) {
						tracker1Vel.v[0] = latestDevicePoses[tracker1ID].vVelocity.v[0];
						tracker1Vel.v[1] = latestDevicePoses[tracker1ID].vVelocity.v[1];
						tracker1Vel.v[2] = latestDevicePoses[tracker1ID].vVelocity.v[2];

						if (tracker2ID != vr::k_unTrackedDeviceIndexInvalid) {
							tracker2Vel.v[0] = latestDevicePoses[tracker2ID].vVelocity.v[0];
							tracker2Vel.v[1] = latestDevicePoses[tracker2ID].vVelocity.v[1];
							tracker2Vel.v[2] = latestDevicePoses[tracker2ID].vVelocity.v[2];
						}
					}
					vr::HmdVector3d_t cont1Vel = { 0, 0, 0 };
					vr::HmdVector3d_t cont2Vel = { 0, 0, 0 };

					cont1Vel.v[0] = latestDevicePoses[controller1ID].vVelocity.v[0];
					cont1Vel.v[1] = latestDevicePoses[controller1ID].vVelocity.v[1];
					cont1Vel.v[2] = latestDevicePoses[controller1ID].vVelocity.v[2];

					if (controller2ID != vr::k_unTrackedDeviceIndexInvalid) {
						cont2Vel.v[0] = latestDevicePoses[controller2ID].vVelocity.v[0];
						cont2Vel.v[1] = latestDevicePoses[controller2ID].vVelocity.v[1];
						cont2Vel.v[2] = latestDevicePoses[controller2ID].vVelocity.v[2];
					}
					try {
						int n = sample.n_rows - 1;
						sample.insert_rows(n, 1);
						sampleResults.insert_rows(n, 1);
						if (n - 2 > maxSampleSize) {
							sample.shed_row(0);
							sampleResults.shed_row(0);
							n = n - 1;
						}
						sample(n, HMD_X_VEL_IDX) = hmdVel.v[0];
						sample(n, HMD_Y_VEL_IDX) = hmdVel.v[1];
						sample(n, HMD_Z_VEL_IDX) = hmdVel.v[2];
						sample(n, HMD_X_ACC_IDX) = hmdAcc.v[0];
						sample(n, HMD_Y_ACC_IDX) = hmdAcc.v[1];
						sample(n, HMD_Z_ACC_IDX) = hmdAcc.v[2];
						sample(n, HMD_YAW_VEL_IDX) = hmdRotVel.v[0];
						sample(n, HMD_PITCH_VEL_IDX) = hmdRotVel.v[1];
						/*sample(n, CNTRL1_Y_VEL_IDX) = std::abs(cont1Vel.v[1]);
						sample(n, CNTRL2_Y_VEL_IDX) = std::abs(cont2Vel.v[1]);
						sample(n, CNTRL1_X_VEL_IDX) = std::abs(cont1Vel.v[0]);
						sample(n, CNTRL2_X_VEL_IDX) = std::abs(cont2Vel.v[0]);
						sample(n, CNTRL1_Z_VEL_IDX) = std::abs(cont1Vel.v[2]);
						sample(n, CNTRL2_Z_VEL_IDX) = std::abs(cont2Vel.v[2]);
						sample(n, TRKR1_Y_VEL_IDX) = tracker1Vel.v[1];
						sample(n, TRKR2_Y_VEL_IDX) = tracker2Vel.v[1];*/
						if (n >= 2) {
							arma::rowvec predictions;
							arma::mat sample_input = sample.t();// sample.submat(n, 0, n, TOUCH_VAL_IDX - 9);
							//sample_input = sample_input.t();
							//LOG(INFO) << "sample input: " << sample_input;
							vrLocoRegress.Predict(sample_input, predictions);
							//LOG(INFO) << "Predicted HMD results: " << predictions;
							if (predictions(0) >= 0) {
								LOG(INFO) << "Predicted a step: " << predictions(0);
							}
							else {
								LOG(INFO) << "Predicted not a step: " << predictions(0);
							}
							sampleResults(n) = predictions(0);
							validInputSample = 0;
							for (int i = n; i >= 0; i--) {
								if (sampleResults(i) >= 0) {
									if (validInputSample < maxSampleSize) {
										validInputSample++;
									}
								}
								else if (i > (n-(n/4)) && validInputSample > 0) {
									validInputSample--;
								}
							}
						}
					}
					catch (std::exception& e) {
						LOG(INFO) << "Exception caught while colleting data sample: " << e.what();
					}
					try {
						if (validInputSample > 3) {
							validSample = true;
							float weightedInputValue = 0.0;
							int n = sampleResults.n_rows - 1;
							for (int i = n; i > 0; i--) {
								weightedInputValue += sampleResults(i)*(((float)i) / ((float)n));
							}
							lastValidSampleMKi = ((int)dataModel.n_cols*weightedInputValue);
							float nextTouch = weightedInputValue;
							float temp = sNValidTouch;
							//sNValidTouch = nextTouch;
							if (nextTouch >= sNValidTouch) {
								sNValidTouch = sNValidTouch + (timeStep*(nextTouch - sNValidTouch)*2.0);
							}
							else {
								sNValidTouch = sNValidTouch - (timeStep*(sNValidTouch - nextTouch)*6.0);
							}

							if (temp != sNValidTouch) {
								inputStateChanged = true;
							}
						}
						else {
							stopMovement();
						}
						if (inputStateChanged) {
							vr::VRControllerAxis_t axisState;
							if (false && directionDevice != vr::k_unTrackedDeviceIndexInvalid) {
								vr::HmdQuaternion_t qRotation = vrmath::quaternionFromRotationMatrix(latestDevicePoses[directionDevice].mDeviceToAbsoluteTracking);
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
							if (gameType->useAxis) {
								axisState.x = 0;
								float touch = 0;
								if (sNValidTouch > 0.5) {
									float normTouch = (sNValidTouch - 0.5) / 0.5;
									touch = (normTouch * (maxTouch - midTouch)) + midTouch;
								}
								else if (sNValidTouch < 0.501) {
									float normTouch = (sNValidTouch / 0.5);
									touch = (normTouch * (midTouch - minTouch)) + minTouch;
								}
								axisState.y = touch;
								if (false && directionDevice != vr::k_unTrackedDeviceIndexInvalid) {
									axisState.x = minTouch * touchX;
									axisState.y = axisState.y * touchY;
								}
								try {
									if (gameType->useClick && !gameType->alwaysHeld) {
										if (sNValidTouch > 0.999) {
											pressedFlag = true;
										}
										else {
											pressedFlag = false;
										}
									}
									applyAxisMovement(axisState);
								}
								catch (std::exception& e) {
									LOG(INFO) << "Exception caught while applying virtual step movement: " << e.what();
								}
							}
							//else if (inputType == 9999) {
							//	applyClickMovement();
							//}
							else if (gameType->inputType == InputType::grip) {
								applyGripMovement();
							}
							if (gameType->inputType == InputType::keyWASD || gameType->inputType == InputType::keyArrow) {
								applyKeyMovement();
							}
							inputStateChanged = false;
						}
					}
					catch (std::exception& e) {
						LOG(INFO) << "Exception caught while sending input: " << e.what();
					}
				}
			}
		}
	}

	void WalkInPlaceTabController::clearClickedFlag() {
		uint32_t deviceId = ovrwipCNTRLID;
		try {
			if (gameType->useAxis) {
				vr::VRControllerAxis_t axisState;
				axisState.x = 0;
				axisState.y = 0;
				try {
					vrwalkinplace::VRWalkInPlace vrwalkinplace;
					vrwalkinplace.connect();
					if (gameType->inputType == InputType::touchpad) {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
						pressedFlag = false;
						vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					else {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
						pressedFlag = false;
						vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_IndexController_JoyStick, axisState);
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
					}
				}
				catch (std::exception& e) {
					//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
				}
			}
		}
		catch (std::exception& e) {
			//LOG(INFO) << "Exception caught while applying virtual axis movement: " << e.what();
		}
	}

	void WalkInPlaceTabController::stopMovement() {
		try {
			if (validSample) {
				stopCallCount = 0;
			}
			if (stopCallCount < 20) {
				stopCallCount++;
				uint32_t deviceId = ovrwipCNTRLID;
				if (gameType->useAxis) {
					vr::VRControllerAxis_t axisState;
					axisState.x = 0;
					axisState.y = 0;
					try {
						vrwalkinplace::VRWalkInPlace vrwalkinplace;
						vrwalkinplace.connect();
						if (gameType->inputType == InputType::touchpad) {
							if (gameType->useClick) {
								vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
								pressedFlag = false;
							}
							vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
							vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
						}
						else {
							if (gameType->useClick) {
								vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
								pressedFlag = false;
							}
							vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_IndexController_JoyStick, axisState);
							vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);

						}
					}
					catch (std::exception& e) {
						//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
					}
				}
				else if (gameType->inputType == InputType::grip) {
					try {
						vrwalkinplace::VRWalkInPlace vrwalkinplace;
						vrwalkinplace.connect();
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Grip, 0.0);
					}
					catch (std::exception& e) {
						//LOG(INFO) << "Exception caught while applying virtual grip movement: " << e.what();
					}
				}
				/*else if (gameType == 9999) { //click only disabled atm
					try {
						vrwalkinplace::VRWalkInPlace vrwalkinplace;
						vrwalkinplace.connect();
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					catch (std::exception& e) {
						//LOG(INFO) << "Exception caught while stopping virtual teleport movement: " << e.what();
					}
				}*/
				else if (gameType->inputType == InputType::keyWASD) {
#if defined _WIN64 || defined _LP64
					INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x57, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(2, input, sizeof(INPUT));
#else
#endif
				}
				else if (gameType->inputType == InputType::keyArrow) {
#if defined _WIN64 || defined _LP64
					INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x26, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(2, input, sizeof(INPUT));
#else
#endif
				}
			}
			validSample = false;
			lastValidSampleMKi = 0;
			sNValidTouch = 0;
		}
		catch (std::exception &e) {
			LOG(INFO) << "error when attempting to stop movement: " << e.what();
		}
	}

	void WalkInPlaceTabController::stopClickMovement() {
		uint32_t deviceId = ovrwipCNTRLID;
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
		uint32_t deviceId = ovrwipCNTRLID;
		vr::VRControllerAxis_t axisState;
		axisState.x = 0;
		axisState.y = 1;
		applyAxisMovement(axisState);
	}

	void WalkInPlaceTabController::applyAxisMovement(vr::VRControllerAxis_t axisState) {
		uint32_t deviceId = ovrwipCNTRLID;
		try {
			vrwalkinplace::VRWalkInPlace vrwalkinplace;
			vrwalkinplace.connect();
			if (gameType->inputType == InputType::touchpad) {
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
			}
			else {
				vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
				vrwalkinplace.openvrAxisEvent(deviceId, vr::k_EButton_IndexController_JoyStick, axisState);
			}
			if (gameType->useClick) {
				if (pressedFlag || gameType->alwaysHeld) {
					if (gameType->inputType == InputType::touchpad) {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					else {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
					}
				}
				else {
					if (gameType->inputType == InputType::touchpad) {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					else {
						vrwalkinplace.openvrButtonEvent(vrwalkinplace::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
					}
				}

			}
		}
		catch (std::exception& e) {
			//LOG(INFO) << "Exception caught while applying virtual axis movement: " << e.what();
		}
	}

	void WalkInPlaceTabController::applyClickMovement() {
		uint32_t deviceId = ovrwipCNTRLID;
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

	void WalkInPlaceTabController::applyGripMovement() {
		uint32_t deviceId = ovrwipCNTRLID;
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

	void WalkInPlaceTabController::applyKeyMovement() {
		if (gameType->inputType == InputType::keyWASD) {
			if (pressedFlag) {
#if defined (_WIN64) || !defined (_LP64)
				INPUT input[2];
				input[0].type = INPUT_KEYBOARD;
				input[0].ki.wVk = 0;
				input[0].ki.wScan = MapVirtualKey(0x57, 0);
				input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
				input[0].ki.time = 0;
				input[0].ki.dwExtraInfo = 0;
				SendInput(1, input, sizeof(INPUT));
#else
#endif
			}
			pressedFlag = false;
		}
		else if (gameType->inputType == InputType::keyArrow) {
			if (pressedFlag) {
#if defined (_WIN64) || !defined (_LP64)
				INPUT input[2];
				input[0].type = INPUT_KEYBOARD;
				input[0].ki.wVk = 0;
				input[0].ki.wScan = MapVirtualKey(0x26, 0);
				input[0].ki.dwFlags = KEYEVENTF_SCANCODE;
				input[0].ki.time = 0;
				input[0].ki.dwExtraInfo = 0;
				SendInput(1, input, sizeof(INPUT));
#else
#endif
			}
			pressedFlag = false;
		}
	}

} // namespace walkinplace
