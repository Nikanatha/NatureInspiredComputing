#ifndef __Controller_H__
#define __Controller_H__

#include <thread>
#include "KheperaUtility.h"

struct SNode { Int8 center; double lWeight; double rWeight; };

#define INPUT_COUNT 8
#define NODE_COUNT 100
#define TRAINING_CYCLES 10000

class CController
{
public:
	CController(CKheperaUtility* pUtil);
	~CController();

	void Start();
	void Stop();

private:
	void Run();
	SIOSet Evaluate(Int8 sensors);
	void Adapt(SIOSet ideal);

	// rbf network functions
	double RbfBase(Int8 sensors, Int8 nodeCenter);
	void CreateTrainingData();
	void Train();

private:
	CKheperaUtility* m_pUtil;
	std::thread* m_pThread;
	bool m_bStopFlag;

	// rbf network
	std::vector<SNode> m_NetworkNodes;
	double m_Sigma;
	double m_LearnWeight;
	std::vector<SIOSet> m_TrainingData;
};

#endif