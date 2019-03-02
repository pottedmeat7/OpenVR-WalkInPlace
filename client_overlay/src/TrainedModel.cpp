#include "TrainedModel.h"

TrainedModel::TrainedModel() {
}

//TrainedModel::~TrainedModel() {
//}

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

void TrainedModel::train(arma::mat dataModel) {
	for (int i = 0; i < dataModel.n_rows; i++) {
		float dT = (1.0 / 20.0);
		for (int j = 0; j < dataModel.n_cols; j++) {
			float currentScale = dataModel(i, dataModel.n_cols - 2);
			float hmdY = dataModel(i, 0);
			float cntrl1Y = dataModel(i, 1);
			float cntrl2Y = dataModel(i, 2);
			saveHMDSample(hmdY, currentSampleTime);
			saveCNTRLSample(cntrl1Y, cntrl2Y, currentSampleTime, (currentScale < 0.5 ? 0 : (currentScale < 1.0 ? 1 : 2)));
			currentSampleTime += dT;
		}
	}
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
