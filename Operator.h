#ifndef __Operator_H__
#define __Operator_H__

#include "ThreadedUtilityUser.h"

class COperator : public CThreadedUtilityUser
{
public:
	COperator(CKheperaUtility* pUtil);

protected:
	virtual void DoCycle();

private:
	CSpeed Noisy(CSpeed target);
};

#endif