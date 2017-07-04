#ifndef __ValueSystem_H__
#define __ValueSystem_H__

#include "ThreadableBase.h"

class CValueSystem : public CThreadableBase
{
public:
	CValueSystem(CKheperaUtility* pUtil);

protected:
	virtual void DoCycle();

private:
	SIOSet Correct(SIOSet calculated);
	//double SpeedLimit(Int8 sensors);
	int SafetyDistance(double straightSpeed);
};

#endif