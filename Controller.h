#ifndef __Controller_H__
#define __Controller_H__

#include "ThreadableBase.h"

struct SNode { Int8 center; double lWeight; double rWeight; double activity; };

#define INPUT_COUNT 6
#define NODE_COUNT 20
#define TRAINING_CYCLES 100
#define NODE_ACTIVITY_DECAY_FACTOR 0.8
#define TOTAL_ACTIVATION_LIMIT 1

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
	void Forget();

private:
	std::vector<SNode> m_NetworkNodes;
	double m_Sigma;
	double m_LearnWeight;
	std::vector<SIOSet> m_TrainingData;
};

#endif