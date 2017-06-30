#ifndef __KheperaUtility_H__
#define __KheperaUtility_H__

#include <random>

#include "KheperaInterface.h"

// custom types
struct SSpeed
{
	double left;
	double right;
	SSpeed(double l = 0, double r = 0) { left = l; right = r; }
};

struct SIOSet
{
	Int8 sensors;
	SSpeed speed;
	SIOSet(Int8 sens = Int8(), SSpeed spd = SSpeed()) { sensors = sens; speed = spd; }
};


// value defines
#define MIN_SENSOR_VAL -1024
#define MAX_SENSOR_VAL 0
#define SENSOR_VAL_RANGE (MAX_SENSOR_VAL - MIN_SENSOR_VAL)
#define MAX_SPEED 30

class CKheperaUtility
{
public:
	CKheperaUtility();
	~CKheperaUtility();

	// for khepera interaction
	Int8 GetSensorData();
	void SetSpeed(Int2 newSpeed);

	// for controller output
	void SetNetworkResult(SIOSet results);
	SIOSet GetLastNetworkResult();

	// for value system output 
	void SetCorrectedResult(SIOSet results);
	SIOSet GetLastCorrectedResult();

	// randomness
	double GetUniformRandom(double max = 1, double min = 0);

private:
	KheperaInterface* m_pKhep;
	std::default_random_engine m_rGenerator;
	SIOSet m_LastResult;
	SIOSet m_LastCorrectedResult;

	bool m_bReading;
	bool m_bWriting;
};

#endif