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
