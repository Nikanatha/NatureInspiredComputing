#ifndef __ValueSystem_H__
#define __ValueSystem_H__

#include "ThreadableBase.h"
#include "BraitenbergWeights.h"

class CValueSystem : public CThreadableBase
{
public:
	CValueSystem(CKheperaUtility* pUtil, CBraitenbergWeights* pWeights);

protected:
	virtual void DoCycle();

private:
	double HistoryFitness(std::vector<SIOSet> history);
	void Correct(std::vector<SIOSet> history);

private:
	CBraitenbergWeights* m_pControllerWeights;
};

#endif