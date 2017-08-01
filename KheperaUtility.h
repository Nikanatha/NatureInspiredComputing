#ifndef __KheperaUtility_H__
#define __KheperaUtility_H__

#include <random>
#include <mutex>

#include "Common.h"
#include "SensorData.h"
#include "Speed.h"

// types
class KheperaInterface;

struct SIOSet
{
	CSensorData sensors;
	CSpeed speed;
};

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
	void SetNetworkResult(SIOSet results);
	SIOSet GetLastNetworkResult();

	// for value system output 
	void SetCorrectedResult(SIOSet results);
	SIOSet GetLastCorrectedResult();

	// randomness
	double GetUniformRandom(double min = 0, double max = 1);

	// info dumping
	void SetVerbosity(bool bVerbose);

private:
	KheperaInterface* m_pKhep;
	std::default_random_engine m_rGenerator;
	SIOSet m_LastResult;
	SIOSet m_LastCorrectedResult;

	std::mutex m_KheperaMutex;
	std::mutex m_ResultMutex;
	std::mutex m_CorrectedResultMutex;

	bool m_bVerbose;
};

#endif