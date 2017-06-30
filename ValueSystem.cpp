#include "ValueSystem.h"

CValueSystem::CValueSystem(CKheperaUtility * pUtil)
{
}

CValueSystem::~CValueSystem()
{
	Stop();
}

void CValueSystem::Start()
{
	m_bStopFlag = false;
	m_pThread = new std::thread(&CValueSystem::Run, this);
	m_pThread->detach();
}

void CValueSystem::Stop()
{
	m_bStopFlag = true;
}

void CValueSystem::Run()
{
	while (!m_bStopFlag)
	{
		// evaluate current results
		SIOSet controllerResults = m_pUtil->GetLastNetworkResult();
		SIOSet correction = Correct(controllerResults);
		m_pUtil->SetCorrectedResult(correction);
	}
}

SIOSet CValueSystem::Correct(SIOSet calculated)
{
	// TODO: replace current dummy function.
	return calculated;
}
