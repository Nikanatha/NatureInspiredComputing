#ifndef __ValueSystem_H__
#define __ValueSystem_H__

#include "ThreadedUtilityUser.h"
#include "BraitenbergWeights.h"

class CValueSystem : public CThreadedUtilityUser
{
public:
	CValueSystem(CKheperaUtility* pUtil, CBraitenbergWeights* pWeights);

protected:
	virtual void DoCycle();

private:
	double HistoryFitness(std::vector<SIOSet> history);
	void Correct(std::vector<SIOSet> history);

	void GenerateChildren(bool keepParent = false);
	void SelectBest();

private:
	CBraitenbergWeights* m_pControllerWeights;
	double m_ParentFitness;

	double m_Sigma;
	int m_ChildCount;
	std::vector<CBraitenbergWeights> m_Children;
	std::vector<double> m_ChildFitnesses;
};

#endif