#ifndef __ThreadableBase_H__
#define __ThreadableBase_H__

#include <thread>

class CThreadableBase
{
public:
	CThreadableBase(int waitTime = 100);
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
};

#endif