#include "ValueSystem.h"
#include <fstream>

CValueSystem::CValueSystem(CKheperaUtility * pUtil, CBraitenbergWeights* pWeights) : CThreadableBase(pUtil, 10000)
{
	m_pControllerWeights = pWeights;
}

void CValueSystem::DoCycle()
{
	auto hist = m_pUtil->GetNetworkResults();
	Correct(hist);
	m_pUtil->ClearHistory();
}

double CValueSystem::HistoryFitness(std::vector<SIOSet> history)
{
	double fitness = 0;
	double bumpCount = 0;

	for (auto set = history.begin(); set != history.end(); set++)
	{
		if (set->sensors.Collision()) bumpCount++;
	}

	fitness = bumpCount / history.size();
	return fitness;
}

void CValueSystem::Correct(std::vector<SIOSet> history)
{	
	double fitness = HistoryFitness(history);
	
	if (fitness == 0)
	{	// haven't bumped, speed up
		m_pControllerWeights->MultiplySpeed(1.1);
	}
	else
	{
		double sigma = fitness * m_pControllerWeights->back().Velocity();
		std::function<double(double, double)> randFunc = std::bind(&CKheperaUtility::GetGaussianRandom, m_pUtil, std::placeholders::_1, std::placeholders::_2);
		m_pControllerWeights->VarySensorWeights(sigma, randFunc);
	}
}
