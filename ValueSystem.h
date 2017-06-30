#pragma once

#include <thread>
#include "KheperaUtility.h"

class CValueSystem
{
public:
	CValueSystem(CKheperaUtility* pUtil);
	~CValueSystem();

	void Start();
	void Stop();

private:
	void Run();
	SIOSet Correct(SIOSet calculated);

private:
	CKheperaUtility* m_pUtil;
	std::thread* m_pThread;
	bool m_bStopFlag;
};

