#include <iostream>
#include <iomanip>

#include "KheperaUtility.h"
#include "KheperaInterface.h"

CKheperaUtility::CKheperaUtility()
{
	bool sim = false;
#ifdef SIM_ONLY
	sim = true;
#endif // SIM_ONLY


	m_pKhep = new KheperaInterface("/dev/ttyUSB0", sim);
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
	ScopedMutexLocker lock(m_ResultMutex);
	m_NetworkResults.push_back(results);

	// output info
	if (m_bVerbose)
	{
        std::cout << "Controller's results:" << std::endl;
		results.sensors.Dump();        
		std::cout << " ==> Left: " << (double)((int)(10 * results.speed.Left())) / 10.0 << " Right: " << (double)((int)(10 * results.speed.Right())) / 10.0;
        std::cout << std::endl;
	}
}

SIOSet CKheperaUtility::GetLastNetworkResult()
{
	ScopedMutexLocker lock(m_ResultMutex);
	if (m_NetworkResults.size() == 0) return SIOSet();
	return (m_NetworkResults.back());
}

std::vector<SIOSet> CKheperaUtility::GetNetworkResults()
{
	ScopedMutexLocker lock(m_ResultMutex);
	return m_NetworkResults;
}

void CKheperaUtility::ClearHistory()
{
	ScopedMutexLocker lock(m_ResultMutex);
	SIOSet last = m_NetworkResults.back();
	m_NetworkResults.clear();
	m_NetworkResults.push_back(last);
}

double CKheperaUtility::GetUniformRandom(double min, double max)
{
	std::uniform_real_distribution<double> rnd(min, max);
	return rnd(m_rGenerator);
}

double CKheperaUtility::GetGaussianRandom(double sigma, double mean)
{
	std::normal_distribution<> rnd(mean, sigma);
	return rnd(m_rGenerator);
}

void CKheperaUtility::SetVerbosity(bool bVerbose)
{
	m_bVerbose = bVerbose;
}
