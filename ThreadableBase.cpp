#include <chrono>

#include "ThreadableBase.h"

CThreadableBase::CThreadableBase(CKheperaUtility * pUtil, int waitTime)
{
	m_pUtil = pUtil;
	m_Wait = waitTime;
}

CThreadableBase::~CThreadableBase()
{
	Stop();
}

void CThreadableBase::Start()
{
	m_bStopFlag = false;
	m_pThread = new std::thread(&CThreadableBase::Run, this);
	m_pThread->detach();
}

void CThreadableBase::Stop()
{
	m_bStopFlag = true;
}

void CThreadableBase::Run()
{
	while (!m_bStopFlag)
	{
		DoCycle();
		std::this_thread::sleep_for(std::chrono::milliseconds(m_Wait));
	}
}
