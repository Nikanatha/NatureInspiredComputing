#include <fstream>
#include <sstream>

#include <algorithm>
#include <iostream>
#include "Controller.h"

#define TOTAL_ACTIVATION_LIMIT 0.5

CController::CController(CKheperaUtility * pUtil, CRbfSettings* pSettings) : CThreadableBase(pUtil)
{
	m_pSettings = pSettings;
}

void CController::LoadNodesFromFile(std::string path)
{
	// Delete current nodes
	m_NetworkNodes.clear();

	std::fstream file;
	std::string line;

	file.open(path);
	if (file.is_open())
	{
		while (getline(file, line))
		{
			std::stringstream ss(line);

			CSensorData sensors;
			int sval;
			for (int d = 0; d <= (int)Direction_Back; d++)
			{
				ss >> sval;
				sensors[(EDirection)d] = SValue(sval);
			}

			CSpeed speed;
			double left;
			double right;
			
			ss >> left >> right;
			speed.SetComponents(left, right);

			AddNode(sensors, speed);
		}

	}
	else std::cout << "Unable to open file";

	file.close();
}

void CController::SaveNodesToFile(std::string path)
{
	std::ofstream file;
	file.open(path, std::ios_base::trunc);

	for (auto it = m_NetworkNodes.begin(); it != m_NetworkNodes.end(); it++)
	{

		if (file.is_open())
		{
			for (int d = 0; d <= (int)Direction_Back; d++)
			{
				file << it->Center()[(EDirection)d].sensor << " ";
			}

			file << it->Weight().Left() << " " << it->Weight().Right() << std::endl;
		}
		else std::cout << "Unable to open file";

	}
	
	file.close();
}

void CController::DoCycle()
{
	// evaluate current sensor data
	CSensorData sensorData = m_pUtil->GetSensorData();

#ifdef SIM_ONLY
	sensorData = CSensorData(Int8({ {
			(int)round(m_pUtil->GetUniformRandom(0, 1024)),
			(int)round(m_pUtil->GetUniformRandom(0, 1024)),
			(int)round(m_pUtil->GetUniformRandom(0, 1024)),
			(int)round(m_pUtil->GetUniformRandom(0, 1024)),
			(int)round(m_pUtil->GetUniformRandom(0, 1024)),
			(int)round(m_pUtil->GetUniformRandom(0, 1024))
			} }));
#endif

	SIOSet result = Evaluate(sensorData);
	m_pUtil->AddNetworkResult(result);

	// get value system's correction
	SIOSet ideal = m_pUtil->GetLastCorrectedResult();
	Adapt(ideal);

	// check for surplus of nodes
	if (m_NetworkNodes.size() > m_pSettings->MaxNodes)
	{
		Forget();
	}
}

void CController::Adapt(SIOSet ideal)
{
	double activation = 0;
	CSpeed current;

	for (int n = 0; n < m_NetworkNodes.size(); n++)
	{
		double act;
		CSpeed out;
		act = m_NetworkNodes[n].Calculate(ideal.sensors, out);
		current += out;
		activation += act;
	}

	if (activation > 0)	current /= activation;

	for (int n = 0; n < m_NetworkNodes.size(); n++)
	{
		m_NetworkNodes[n].Adapt(ideal.sensors, ideal.speed - current);
	}
}

void CController::AddNode(CSensorData sensors, CSpeed speed)
{
	CNode node(sensors, speed);
    
    //if(dist > MAX_DIMENSION_DISTANCE || m_NetworkNodes.size() == 0)
    {
        m_NetworkNodes.push_back(node);
        std::cout << "Added new ";
		node.Dump();
        std::cout << std::endl;
    }
}

void CController::Forget()
{
    printf("Too much knowledge! Must forget!");
	std::sort(m_NetworkNodes.begin(), m_NetworkNodes.end(), CNode::CompareActivity);
    
    int count = 0;
    for (auto it = m_NetworkNodes.begin(); it != m_NetworkNodes.end(); it++)
    {
        count++;
        if(count > m_pSettings->MaxNodes)
        {
			std::cout << "Forgetting ";
			it->Dump();
			std::cout << std::endl;
        }
    }
    
	m_NetworkNodes.resize(m_pSettings->MaxNodes);
}

SIOSet CController::Evaluate(CSensorData sensors)
{
	double activation = 0;
	CSpeed speed;

	for (int n = 0; n < m_NetworkNodes.size(); n++)
	{
		double act;
		CSpeed out;
		act = m_NetworkNodes[n].Activate(sensors, out);
		speed += out;
		activation += act;
	}

	if (activation > 0) speed /= activation;

	SIOSet result;
	result.sensors = sensors;
	result.speed = speed;

	// add extra nodes if activation is too low
	if (activation < TOTAL_ACTIVATION_LIMIT)
	{
		AddNode(sensors, CSpeed());
	}

	return result;
}

void CController::ListNodes()
{
    std::cout << "These are my nodes:" << std::endl;
    for (auto it = m_NetworkNodes.begin(); it != m_NetworkNodes.end(); it++)
    {
		it->Dump();
        std::cout << std::endl;
    }
}
