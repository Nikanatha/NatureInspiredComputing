#ifndef __ThreadedUtilityUser_H__
#define __ThreadedUtilityUser_H__

#include "ThreadableBase.h"
#include "KheperaUtility.h"

class CThreadedUtilityUser : public CThreadableBase
{
public:
	CThreadedUtilityUser(CKheperaUtility* pUtil, int waitTime = 100);

protected:
	virtual void DoCycle() = 0;

	CKheperaUtility* m_pUtil;
};

#endif
