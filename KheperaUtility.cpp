#include <iostream>
#include <iomanip>

#include "KheperaUtility.h"
#include "KheperaInterface.h"

CKheperaUtility::CKheperaUtility()
{
	m_pKhep = new KheperaInterface("/dev/ttyUSB0");
	m_pKhep->setLEDState(0, LEDState::On);

	MaxSpeed = 20;
		
	m_bVerbose = false;
    
    m_rGenerator = std::default_random_engine(std::random_device{}());
}

CKheperaUtility::~CKheperaUtility()
{
	m_pKhep->setLEDState(0, LEDState::Off);
	m_pKhep->setSpeed(0, 0);
	delete m_pKhep;
}

CSensorData CKheperaUtility::GetSensorData()
{
	ScopedMutexLocker lock(m_KheperaMutex);
	Int8 data;

	try
	{
		data = m_pKhep->getProximitySensors();
	}
	catch (...)
	{
		printf("Error reading sensor data!");
		return GetLastNetworkResult().sensors;
	}

	return CSensorData(data);
}

void CKheperaUtility::SetSpeed(int left, int right)
{
	ScopedMutexLocker lock(m_KheperaMutex);
	try
	{
		m_pKhep->setSpeed(left, right);
	}
	catch (...)
	{
		printf("Error setting speed!");
	}	
}

void CKheperaUtility::AddNetworkResult(SIOSet results)
{
	//void* t = this;
	ScopedMutexLocker lock(m_ResultMutex);
	m_NetworkResults.push_back(results);

	// output info
	if (m_bVerbose)
	{
        std::cout << "Controller's results:" << std::endl;
		results.sensors.Dump();        
        std::cout << " ==> Angle: " << results.speed.Angle() << " Speed: " << results.speed.Velocity();
        std::cout << std::endl;
	}

	while (m_NetworkResults.size() > HISTORY_LENGTH)
	{
		m_NetworkResults.pop_back();
	}
}

SIOSet CKheperaUtility::GetLastNetworkResult()
{
	//void* t = this;
	ScopedMutexLocker lock(m_ResultMutex);
	return (m_NetworkResults.back());
}

std::vector<SIOSet> CKheperaUtility::GetNetworkResults()
{
	ScopedMutexLocker lock(m_ResultMutex);
	return m_NetworkResults;
}

void CKheperaUtility::SetCorrectedResult(SIOSet results)
{
	ScopedMutexLocker lock(m_CorrectedResultMutex);
	m_LastCorrectedResult = results;

	// output info
	if (m_bVerbose)
	{
        std::cout << "ValueSystem's results:" << std::endl;
		results.sensors.Dump();
		std::cout << " ==> Angle: " << results.speed.Angle() << " Speed: " << results.speed.Velocity();
        std::cout << std::endl;
	}
}

SIOSet CKheperaUtility::GetLastCorrectedResult()
{
	ScopedMutexLocker lock(m_CorrectedResultMutex);
	return m_LastCorrectedResult;
}

double CKheperaUtility::GetUniformRandom(double min, double max)
{
	std::uniform_real_distribution<double> rnd(min, max);
	return rnd(m_rGenerator);
}

void CKheperaUtility::SetVerbosity(bool bVerbose)
{
	m_bVerbose = bVerbose;
}
