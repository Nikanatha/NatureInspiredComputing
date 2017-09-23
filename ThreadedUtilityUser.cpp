#include "ThreadedUtilityUser.h"

CThreadedUtilityUser::CThreadedUtilityUser(CKheperaUtility * pUtil, int waitTime) : CThreadableBase(waitTime)
{
	m_pUtil = pUtil;
}
