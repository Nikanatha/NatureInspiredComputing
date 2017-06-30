#ifndef __Operator_H__
#define __Operator_H__

#include <thread>
#include "KheperaUtility.h"

class COperator
{
public:
	COperator(CKheperaUtility* pUtil);
	~COperator();

	void Start();
	void Stop();

private:
	void Run();
	Int2 NoiseRound(SSpeed target);

private:
	CKheperaUtility* m_pUtil;
	std::thread* m_pThread;
	bool m_bStopFlag;
};

#endif