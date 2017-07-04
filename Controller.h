#ifndef __Controller_H__
#define __Controller_H__

#include "ThreadableBase.h"

struct SNode { Int8 center; double lWeight; double rWeight; };

#define INPUT_COUNT 8
#define NODE_COUNT 100
#define TRAINING_CYCLES 100

class CController : public CThreadableBase
{
public:
	CController(CKheperaUtility* pUtil);

protected:
	virtual void DoCycle();

private:
	SIOSet Evaluate(Int8 sensors);
	void Adapt(SIOSet ideal);

	// rbf network functions
	double RbfBase(Int8 sensors, Int8 nodeCenter);
	void CreateTrainingData();
	void Train();

private:
	std::vector<SNode> m_NetworkNodes;
	double m_Sigma;
	double m_LearnWeight;
	std::vector<SIOSet> m_TrainingData;
};

#endif