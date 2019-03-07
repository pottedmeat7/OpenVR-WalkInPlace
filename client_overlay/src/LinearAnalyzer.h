
#include <mlpack/core.hpp>

class LinearAnalyzer
{
private:
	float minError = 1.0;
	float maxDTidx = 1.0;

public:
	LinearAnalyzer();
	~LinearAnalyzer();
	arma::mat computeDelta(arma::mat sample, arma::mat model, int kV);
	bool isWithinVariance(arma::mat sampleDelta, float minErr = 1, float maxDTidx = 1);
};

