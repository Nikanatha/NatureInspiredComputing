#ifndef __Operator_H__
#define __Operator_H__

#include "ThreadableBase.h"

class COperator : public CThreadableBase
{
public:
	COperator(CKheperaUtility* pUtil);

protected:
	virtual void DoCycle();

private:
	Int2 NoiseRound(SSpeed target);
};

#endif