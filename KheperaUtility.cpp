#include "KheperaUtility.h"

CKheperaUtility::CKheperaUtility()
{
	m_pKhep = new KheperaInterface("/dev/ttyUSB0");
	m_LastResult = SIOSet(Int8(), SSpeed(MAX_SPEED, MAX_SPEED));
	m_LastCorrectedResult = SIOSet(Int8(), SSpeed(MAX_SPEED, MAX_SPEED));
}

CKheperaUtility::~CKheperaUtility()
{
	delete m_pKhep;
}

Int8 CKheperaUtility::GetSensorData()
{
	ScopedMutexLocker lock(m_KheperaMutex);
	Int8 data = m_pKhep->getProximitySensors();
	return data;
}

void CKheperaUtility::SetSpeed(Int2 newSpeed)
{
	ScopedMutexLocker lock(m_KheperaMutex);
	m_pKhep->setSpeed(newSpeed.data[0], newSpeed.data[1]);
}

void CKheperaUtility::SetNetworkResult(SIOSet results)
{
	void* t = this;
	ScopedMutexLocker lock(m_ResultMutex);
	m_LastResult = results;

	// output info
	printf("Controller's results:\n P0: %d P1: %d P2: %d P3: %d P4: %d P5: %d P6: %d P7: %d ==> L: %f R: %f \n",
		results.sensors.data[0], 
		results.sensors.data[1], 
		results.sensors.data[2], 
		results.sensors.data[3], 
		results.sensors.data[4], 
		results.sensors.data[5], 
		results.sensors.data[6], 
		results.sensors.data[7], 

		results.speed.left, 
		results.speed.right);
}

SIOSet CKheperaUtility::GetLastNetworkResult()
{
	void* t = this;
	ScopedMutexLocker lock(m_ResultMutex);
	return m_LastResult;
}

void CKheperaUtility::SetCorrectedResult(SIOSet results)
{
	ScopedMutexLocker lock(m_CorrectedResultMutex);
	m_LastCorrectedResult = results;


	// output info
	printf("ValueSystem's ideal results:\n P0: %d P1: %d P2: %d P3: %d P4: %d P5: %d P6: %d P7: %d ==> L: %f R: %f \n",
		results.sensors.data[0],
		results.sensors.data[1],
		results.sensors.data[2],
		results.sensors.data[3],
		results.sensors.data[4],
		results.sensors.data[5],
		results.sensors.data[6],
		results.sensors.data[7],

		results.speed.left,
		results.speed.right);
}

SIOSet CKheperaUtility::GetLastCorrectedResult()
{
	ScopedMutexLocker lock(m_CorrectedResultMutex);
	return m_LastCorrectedResult;
}

double CKheperaUtility::GetUniformRandom(double max, double min)
{
	std::uniform_real_distribution<double> rnd(min, max);
	return rnd(m_rGenerator);
}
