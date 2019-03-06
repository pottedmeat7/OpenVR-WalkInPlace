#include "TrainedModel.h"

TrainedModel::TrainedModel() {
}

//TrainedModel::~TrainedModel() {
//}
float TrainedModel::computeDelta(arma::mat sample, int kV) {
	int kV_set = sample.n_rows > kV ? sample.n_rows - 1 : kV;
	arma::rowvec mN = hmdModel.cols(0, 0);
	arma::rowvec sN = sample.cols(0, 0);
	int mNk = std::floor(mN.n_rows / kV_set);
	int sNk = std::floor(sN.n_rows / kV_set);
	arma::mat dS(sNk,2);
	for (int ki = 0; ki < sNk; ki++) {
		arma::rowvec sKi = sN.rows(ki*kV_set, (ki*kV_set) + sNk - 1);
		for (int mi = 0; mi < mNk; mi++) {
			arma::rowvec mKi = mN.rows((mi*kV_set), (mi*kV_set) + mNk - 1);
			arma::vec dKi = arma::abs(mKi - sKi);
			float dK_i = arma::sum(dKi*dKi);
			if (dK_i < dS(ki,0) ) {
				dS(ki) = (dK_i, (mi*kV_set));
			}
		}
	}
	float dKN = arma::sum(dS.cols(0, 0).each_row);
	arma::rowvec dKN1 = dS.shed_row.cols(0, 1);
	float dKidx = arma::sum(dKN1-dS.shed_rows(dS.n_rows-1,1).cols(0, 1));
	return ((float)sample.n_rows / ((float)delta);
}

void TrainedModel::train(arma::mat dataModel, int kV) {
	int kV_set = kV < 1 ? 1 : kV;
	std::vector<float> stepHMDVari;
	std::vector<float> stepCNTRL1Vari;
	std::vector<float> stepCNTRL2Vari;
	for (int i = 0; i < dataModel.n_rows; i++) {
		float dT = (1.0 / samplePerSec);
		float currentScale = dataModel(i, dataModel.n_cols - 2);
		float hmdY = dataModel(i, 0);
		float cntrl1Y = dataModel(i, 1);
		float cntrl2Y = dataModel(i, 2);
		stepHMDVari.push_back(hmdY);
		stepCNTRL1Vari.push_back(cntrl1Y);
		stepCNTRL2Vari.push_back(cntrl2Y);
		if (i >= kV_set) {
			for (int k = kV_set; k > 0; k--) {
				if (std::abs(hmdY - stepHMDVari.at(kV_set - k)) > maxHMDVariance.at(kV_set - k)) {
					maxHMDVariance.at(kV_set - k) = std::abs(hmdY - stepHMDVari.at(kV_set - k));
				}
				if (std::abs(cntrl1Y - stepCNTRL1Vari.at(kV_set - k)) > maxCNTRLVariance.at(kV_set - k)) {
					maxCNTRLVariance.at(kV_set - k) = std::abs(cntrl1Y - stepCNTRL1Vari.at(kV_set - k));
				}
				if (std::abs(cntrl2Y - stepCNTRL2Vari.at(kV_set - k)) < maxCNTRLVariance.at(kV_set - k)) {
					maxCNTRLVariance.at(kV_set - k) = std::abs(cntrl2Y - stepCNTRL2Vari.at(kV_set - k));
				}
				if (std::abs(hmdY - stepHMDVari.at(kV_set - k)) < minHMDVariance.at(kV_set - k)) {
					minHMDVariance.at(kV_set - k) = std::abs(hmdY - stepHMDVari.at(kV_set - k));
				}
				if (std::abs(cntrl1Y - stepCNTRL1Vari.at(kV_set - k)) < minCNTRLVariance.at(kV_set - k)) {
					minCNTRLVariance.at(kV_set - k) = std::abs(cntrl1Y - stepCNTRL1Vari.at(kV_set - k));
				}
				if (std::abs(cntrl2Y - stepCNTRL2Vari.at(kV_set - k)) < minCNTRLVariance.at(kV_set - k)) {
					minCNTRLVariance.at(kV_set - k) = std::abs(cntrl2Y - stepCNTRL2Vari.at(kV_set - k));
				}
			}
			stepHMDVari.push_back(hmdY);
			stepCNTRL1Vari.push_back(cntrl1Y);
			stepCNTRL2Vari.push_back(cntrl2Y);
			stepHMDVari.erase(stepHMDVari.begin());
			stepCNTRL1Vari.erase(stepCNTRL1Vari.begin());
			stepCNTRL2Vari.erase(stepCNTRL2Vari.begin());
		}
		else {
			stepHMDVari.push_back(hmdY);
			stepCNTRL1Vari.push_back(cntrl1Y);
			stepCNTRL2Vari.push_back(cntrl2Y);
			maxHMDVariance.push_back(0);
			minHMDVariance.push_back(9999);
			maxCNTRLVariance.push_back(0);
			minCNTRLVariance.push_back(9999);
		}
		saveHMDSample(hmdY, currentSampleTime);
		saveCNTRLSample(cntrl1Y, cntrl2Y, currentSampleTime, (currentScale < 0.5 ? 0 : (currentScale < 1.0 ? 1 : 2)));
		currentSampleTime += dT;
		hmdModel(i, 0) = dataModel(i, 0);
		hmdModel(i, 1) = dataModel(i, dataModel.n_cols - 1);
		cntrl1Model(i, 0) = dataModel(i, 1);
		cntrl1Model(i, 1) = dataModel(i, dataModel.n_cols - 1);
		cntrl2Model(i, 0) = dataModel(i, 2);
		cntrl2Model(i, 1) = dataModel(i, dataModel.n_cols - 1);
		trkr1Model(i, 0) = dataModel(i, 3);
		trkr1Model(i, 1) = dataModel(i, dataModel.n_cols - 1);
		trkr2Model(i, 0) = dataModel(i, 4);
		trkr2Model(i, 1) = dataModel(i, dataModel.n_cols - 1);
	}
	/*hmdModel = arma::sort(hmdModel, "ascend", 0);
	cntrl1Model = arma::sort(cntrl1Model, "ascend", 0);
	cntrl2Model = arma::sort(cntrl2Model, "ascend", 0);
	trkr1Model = arma::sort(trkr1Model, "ascend", 0);
	trkr2Model = arma::sort(trkr2Model, "ascend", 0);*/
}

int TrainedModel::isHMDPeak(float hmdY, double now) {
	int mode = -1;
	float hmdACCY = (hmdY - lastHMDVelY) / now < 0 ? -1 : 1;
	if (hmdACCY != lastHMDACCY) {
		if (hmdY > minHMDY) {
			if ((currentSampleTime - lastHMDPeak) <= maxHMDFRQ) {
				if (hmdYPeakCount >= 1) {
					mode = 1;
				}
				hmdYPeakCount++;
			}
		}
		lastHMDPeak = currentSampleTime;
	}
	lastHMDVelY = hmdY;
	lastHMDACCY = hmdACCY;
	return mode;
}

float TrainedModel::isCNTRL1Peak(float cntrlY, double now) {
	float mode = 0;
	float cntrlACC = (cntrlY - lastCNTRL1VelY) / now < 0 ? -1 : 1;
	if (cntrlACC != lastCNTRL1ACCY) {
		if (cntrlY > minCNTRLY_slow) {
			if ((currentSampleTime - lastCNTRL1Peak) <= maxCNTRLFRQ) {
				cntrlPeaks.push_back(cntrlY);
				if (cntrlPeaks.size() > 36) { // 18 peaks for 2 controllers //first 12 are baseline following control scale to average
					cntrlPeaks.erase(cntrlPeaks.begin());
				}
				if (cntrl1YPeakCount >= 1) {
					mode = cntrlY > minCNTRLY_fast ? 3 : (cntrlY > minCNTRLY_medium ? 2 : 1);
				}
				cntrl1YPeakCount++;
			}
		}
		lastHMDPeak = currentSampleTime;
	}
	lastCNTRL1VelY = cntrlY;
	lastCNTRL1ACCY = cntrlACC;
	return mode;
}

void TrainedModel::saveHMDSample(float hmdY, double dT) {
	float hmdACCY = (hmdY - lastHMDVelY) / dT < 0 ? -1 : 1;
	if (initTrainStep < 5) {
		lastHMDACCY = hmdACCY;
		initTrainStep++;
	}
	if (hmdACCY != lastHMDACCY) {
		if (hmdY < minHMDY) {
			minHMDY = hmdY;
		}
		if ((currentSampleTime - lastHMDPeak) > maxHMDFRQ) {
			maxHMDFRQ = (currentSampleTime - lastHMDPeak);
		}
		lastHMDPeak = currentSampleTime;
	}
	lastHMDVelY = hmdY;
	lastHMDACCY = hmdACCY;

}

void TrainedModel::saveCNTRLSample(float cntrl1Y, float cntrl2Y, double dT, int mode) {
	float cntrl1ACC = (cntrl1Y - lastCNTRL1VelY) / dT < 0 ? -1 : 1;
	float cntrl2ACC = (cntrl2Y - lastCNTRL2VelY) / dT < 0 ? -1 : 1;
	if (initTrainStep < 5) {
		lastCNTRL1ACCY = cntrl1ACC;
		lastCNTRL2ACCY = cntrl1ACC;
		initTrainStep++;
	}
	if (cntrl1ACC != lastCNTRL1ACCY) {
		if (mode == 0) {
			if (cntrl1Y < minCNTRLY_slow) {
				minCNTRLY_slow = cntrl1Y;
			}
		}
		else if (mode == 1) {
			if (cntrl1Y < minCNTRLY_medium) {
				minCNTRLY_medium = cntrl1Y;
			}
		}
		else if (mode == 2) {
			if (cntrl1Y < minCNTRLY_fast) {
				minCNTRLY_fast = cntrl1Y;
			}
		}
		if ((currentSampleTime - lastCNTRL1Peak) > maxCNTRLFRQ) {
			maxCNTRLFRQ = (currentSampleTime - lastCNTRL1Peak);
		}
		lastCNTRL1Peak = currentSampleTime;
	}
	if (cntrl2ACC != lastCNTRL2ACCY) {
		if (mode == 0) {
			if (cntrl2Y < minCNTRLY_slow) {
				minCNTRLY_slow = cntrl2Y;
			}
		}
		else if (mode == 1) {
			if (cntrl2Y < minCNTRLY_medium) {
				minCNTRLY_medium = cntrl2Y;
			}
		}
		else if (mode == 2) {
			if (cntrl2Y < minCNTRLY_fast) {
				minCNTRLY_fast = cntrl2Y;
			}
		}
		if ((currentSampleTime - lastCNTRL2Peak) > maxCNTRLFRQ) {
			maxCNTRLFRQ = (currentSampleTime - lastCNTRL2Peak);
		}
		lastCNTRL2Peak = currentSampleTime;
	}
	lastCNTRL1VelY = cntrl1Y;
	lastCNTRL1ACCY = cntrl1ACC;
	lastCNTRL2VelY = cntrl2Y;
	lastCNTRL2ACCY = cntrl2ACC;
}
