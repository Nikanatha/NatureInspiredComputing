#include "ValueSystem.h"
#include <fstream>

#define PREV_FILE "previous.txt"
#define FIT_LIMIT (0.03 / 8)

CValueSystem::CValueSystem(CKheperaUtility * pUtil, CBraitenbergWeights* pWeights) : CThreadedUtilityUser(pUtil, 5000)
{
	m_pControllerWeights = pWeights;
	m_PreviousFitness = -1;
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
		bumpCount += set->sensors.Collision();
	}

	fitness = bumpCount / (8 * history.size()); // average collision over time for 8 sensors
	return fitness;
}

void CValueSystem::Correct(std::vector<SIOSet> history)
{	
	if (history.size() == 0) return;

	double fitness = HistoryFitness(history);
	
	std::cout << "Last fitness = " << m_PreviousFitness << ", current = " << fitness << std::endl;

	if (m_PreviousFitness == -1)
	{
		m_PreviousFitness = fitness;
		m_pControllerWeights->SaveToFile(PREV_FILE);
	}

	if (fitness > m_PreviousFitness)
	{
		m_PreviousFitness = -1;
		m_pControllerWeights->LoadFromFile(PREV_FILE);
	}
	else
	{
		m_pControllerWeights->SaveToFile(PREV_FILE);
		if (fitness < FIT_LIMIT && abs(m_PreviousFitness) < FIT_LIMIT)
		{	// bumped an average of max one sensor 3% of the time, speed up
			m_pControllerWeights->MultiplySpeed(1 + 2 / m_pControllerWeights->back().Velocity());
		}
		m_PreviousFitness = fitness;
		
		if (fitness > 0)
		{
			//double sigma = fitness * m_pControllerWeights->back().Velocity();
			std::function<double(double, double)> randFunc = std::bind(&CKheperaUtility::GetGaussianRandom, m_pUtil, std::placeholders::_1, std::placeholders::_2);
			m_pControllerWeights->VarySensorWeights(fitness, randFunc);
		}
	}
}
