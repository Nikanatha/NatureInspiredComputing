#ifndef __ThreadableBase_H__
#define __ThreadableBase_H__

#include <thread>
#include "KheperaUtility.h"

class CThreadableBase
{
public:
	CThreadableBase(CKheperaUtility* pUtil, int waitTime = 50);
	~CThreadableBase();

	void Start();
	void Stop();

private:
	void Run();
	
	std::thread* m_pThread;
	bool m_bStopFlag;
	int m_Wait;

protected:
	virtual void DoCycle() = 0;

	CKheperaUtility* m_pUtil;
};

#endif