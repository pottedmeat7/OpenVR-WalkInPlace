
#include "LinearAnalyzer.h"

LinearAnalyzer::LinearAnalyzer()
{
}


LinearAnalyzer::~LinearAnalyzer()
{
}

arma::mat LinearAnalyzer::computeDelta(arma::mat sample, arma::mat model, int kV) {
	int kV_set = sample.n_rows > kV ? sample.n_rows - 1 : kV;
	arma::rowvec mN = model.cols(0, 0);
	arma::rowvec sN = sample.cols(0, 0);
	int mNk = std::floor(mN.n_rows / kV_set);
	int sNk = std::floor(sN.n_rows / kV_set);
	arma::mat dS(sNk, 2);
	for (int ki = 0; ki < sNk; ki++) {
		arma::rowvec sKi = sN.rows(ki*kV_set, (ki*kV_set) + sNk - 1);
		for (int mi = 0; mi < mNk; mi++) {
			arma::rowvec mKi = mN.rows((mi*kV_set), (mi*kV_set) + mNk - 1);
			arma::vec dKi = arma::abs(mKi - sKi);
			float dK_i = arma::sum(dKi*dKi);
			if (dK_i < dS(ki, 0)) {
				dS(ki) = (dK_i, (mi*kV_set));
			}
		}
	}
	return dS;
}

bool LinearAnalyzer::isWithinVariance(arma::mat sampleDelta, float minErr, float minDT) {
	return false;
}