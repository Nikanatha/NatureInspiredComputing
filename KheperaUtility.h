#ifndef __KheperaUtility_H__
#define __KheperaUtility_H__

#include <random>
#include <mutex>
#include <vector>

#include "Common.h"
#include "SensorData.h"
#include "Speed.h"

struct SIOSet
{
	CSensorData sensors;
	CSpeed speed;
};

// types
class KheperaInterface;

typedef std::lock_guard<std::mutex> ScopedMutexLocker;

class CKheperaUtility
{
public:
	double MaxSpeed;

public:
	CKheperaUtility();
	~CKheperaUtility();

	// for khepera interaction
	CSensorData GetSensorData();
	void SetSpeed(int left, int right);

	// for controller output
	void AddNetworkResult(SIOSet results);
	SIOSet GetLastNetworkResult();
	std::vector<SIOSet> GetNetworkResults();

	// for value system
	void ClearHistory();

	// randomness
	double GetUniformRandom(double min = 0, double max = 1);
	double GetGaussianRandom(double sigma = 1, double mean = 0);

	// info dumping
	void SetVerbosity(bool bVerbose);

private:
	KheperaInterface* m_pKhep;
	std::default_random_engine m_rGenerator;

	std::vector<SIOSet> m_NetworkResults;
	std::vector<double> m_Fitnesses;
	SIOSet m_LastCorrectedResult;

	std::mutex m_KheperaMutex;
	std::mutex m_ResultMutex;
	std::mutex m_CorrectedResultMutex;

	bool m_bVerbose;
};

#endif