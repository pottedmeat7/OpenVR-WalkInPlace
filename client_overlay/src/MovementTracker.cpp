

class TrainedModel {
private:
	float lastHMDACCY = 9999;
	float lastCNTRL1VelY = 0;
	float lastCNTRL2VelY = 0;
	float lastCNTRL1ACCY = 9999;
	float lastCNTRL2ACCY = 9999;

	float minHMDY = 9999;
	float minCNTRLY_slow = 9999;
	float minCNTRLY_medium = 9999;
	float minCNTRLY_fast = 9999;

	double lastHMDPeak = 0;
	double lastCNTRL1Peak = 0;
	double lastCNTRL2Peak = 0;
	double maxHMDFRQ = 0;
	double maxCNTRLFRQ = 0;
	double currentSampleTime = 0;

public:
	void saveHMDSample(float hmdYVel, double dT);
	void saveCNTRLSample(float cntrl1Y, float cntrl2Y, double dT, int mode);
};
