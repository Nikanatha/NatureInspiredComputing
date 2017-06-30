#include "KheperaUtility.h"

CKheperaUtility::CKheperaUtility()
{
	m_pKhep = new KheperaInterface("/dev/ttyUSB0");
	m_bReading = false;
	m_bWriting = false;
}

CKheperaUtility::~CKheperaUtility()
{
	delete m_pKhep;
}

Int8 CKheperaUtility::GetSensorData()
{
	while (m_bWriting)
	{
		// just waiting for a turn
	}

	m_bReading = true;
	Int8 data = m_pKhep->getProximitySensors();
	m_bReading = false;
	return data;
}

void CKheperaUtility::SetSpeed(Int2 newSpeed)
{
	while (m_bReading)
	{
		// just waiting for a turn
	}

	m_bWriting = true;
	m_pKhep->setSpeed(newSpeed.data[0], newSpeed.data[1]);
	m_bWriting = false;
}

void CKheperaUtility::SetNetworkResult(SIOSet results)
{
	m_LastResult = results;
}

SIOSet CKheperaUtility::GetLastNetworkResult()
{
	return m_LastResult;
}

void CKheperaUtility::SetCorrectedResult(SIOSet results)
{
	m_LastCorrectedResult = results;
}

SIOSet CKheperaUtility::GetLastCorrectedResult()
{
	return m_LastCorrectedResult;
}

double CKheperaUtility::GetUniformRandom(double max, double min)
{
	std::uniform_real_distribution<double> rnd(min, max);
	return rnd(m_rGenerator);
}
