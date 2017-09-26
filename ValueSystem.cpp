#include "ValueSystem.h"
#include <fstream>

#define PREV_FILE "previous.txt"
#define FIT_LIMIT (0.03 / 8)

CValueSystem::CValueSystem(CKheperaUtility * pUtil, CBraitenbergWeights* pWeights) : CThreadedUtilityUser(pUtil, 10000)
{
	m_pControllerWeights = pWeights;
	m_ParentFitness = -1;
	m_Sigma = 1;
}

void CValueSystem::DoCycle()
{
	auto hist = m_pUtil->GetNetworkResults();
	auto fit = HistoryFitness(hist);
	m_pUtil->ClearHistory();

	if (m_Children.empty())
	{
		m_ParentFitness = fit;
		GenerateChildren();
	}
	else
	{
		m_ChildFitnesses.push_back(fit);
		if (m_ChildFitnesses.size() < m_Children.size())
		{
			*m_pControllerWeights = m_Children[m_ChildFitnesses.size()];
		}
		else
		{
			SelectBest();
		}
	}

}

double CValueSystem::HistoryFitness(std::vector<SIOSet> history)
{
	double fitness = 0;
	double bumpCount = 0;
	double speed = 0;
	double curving = 0;

	for (auto set = history.begin(); set != history.end(); set++)
	{
		bumpCount += set->sensors.Collision();
		speed += set->speed.Velocity();
		curving += abs(set->speed.Left() - set->speed.Right());
	}

	fitness = exp(-speed/curving) + bumpCount / (8 * history.size()); // average collision over time for 8 sensors
	return fitness;
}

void CValueSystem::Correct(std::vector<SIOSet> history)
{	
	if (history.size() == 0) return;

	double fitness = HistoryFitness(history);
	
	std::cout << "Last fitness = " << m_ParentFitness << ", current = " << fitness << std::endl;

	if (m_ParentFitness == -1)
	{
		m_ParentFitness = fitness;
		m_pControllerWeights->SaveToFile(PREV_FILE);
	}

	if (fitness > m_ParentFitness)
	{
		m_ParentFitness = -1;
		m_pControllerWeights->LoadFromFile(PREV_FILE);
	}
	else
	{
		m_pControllerWeights->SaveToFile(PREV_FILE);
		if (fitness < FIT_LIMIT && abs(m_ParentFitness) < FIT_LIMIT)
		{	// bumped an average of max one sensor 3% of the time, speed up
			m_pControllerWeights->MultiplySpeed(1 + 2 / m_pControllerWeights->back().Velocity());
		}
		m_ParentFitness = fitness;
		
		if (fitness > 0)
		{
			//double sigma = fitness * m_pControllerWeights->back().Velocity();
			std::function<double(double, double)> randFunc = std::bind(&CKheperaUtility::GetGaussianRandom, m_pUtil, std::placeholders::_1, std::placeholders::_2);
			m_pControllerWeights->VarySensorWeights(fitness, randFunc);
		}
	}
}

void CValueSystem::GenerateChildren(bool keepParent)
{
	m_Children.clear();
	m_ChildFitnesses.clear();

	std::function<double(double, double)> randFunc = std::bind(&CKheperaUtility::GetGaussianRandom, m_pUtil, std::placeholders::_1, std::placeholders::_2);

	if (keepParent)
	{
		m_Children.push_back(*m_pControllerWeights);
		m_ChildFitnesses.push_back(m_ParentFitness);
	}

	for (int i = 0; i < m_ChildCount; i++)
	{
		CBraitenbergWeights child = *m_pControllerWeights;
		child.VarySensorWeights(m_Sigma, randFunc);
		m_Children.push_back(child);
	}
}

void CValueSystem::SelectBest()
{
	int best = 0;
	int improvements = 0;

	for (int i = 0; i < m_Children.size(); i++)
	{
		if (m_ChildFitnesses[i] < m_ParentFitness) improvements++;
		if (m_ChildFitnesses[i] < m_ChildFitnesses[best]) best = i;
	}
	
	// set new parent
	m_ParentFitness = m_ChildFitnesses[best];
	*m_pControllerWeights = m_Children[best];
	m_pControllerWeights->SaveToFile("best.txt");

	// adapt sigma
	double limit = (double)m_ChildCount / 5;
	if (improvements < floor(limit)) m_Sigma *= 0.8;
	if (improvements > ceil(limit)) m_Sigma *= 1.2;

	m_Children.clear();
	m_ChildFitnesses.clear();
}
