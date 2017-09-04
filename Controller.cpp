#include <fstream>
#include <sstream>

#include <algorithm>
#include <iostream>
#include "Controller.h"

CController::CController(CKheperaUtility * pUtil, CRbfSettings* pSettings) : CThreadableBase(pUtil)
{
	m_pSettings = pSettings;
	//RebuildNetwork();

	int steps = 0;
	if (steps>0)
	{
		for (int i = 0; i < pow(steps, (int)Direction_Back + 1); i++)
		{
			CSensorData center;
			int mod;
			int div;

			div = i;
			for (int d = (int)Direction_FrontLeft; d <= (int)Direction_Back + 1; d++)
			{
				mod = div%steps;
				div = div / steps;

				center[(EDirection)(d - 1)] = mod * 1000 / (steps - 1);
			}

			m_NetworkNodes.AddNode(center, CSpeed(m_pUtil->GetUniformRandom(0, 20), m_pUtil->GetUniformRandom(-PI / 2, PI / 2)));
		}
	}
	else
	{
		for (int f = (int)Direction_Left; f < (int)Direction_Back + 1; f++)
		{
			CSensorData center;
			for (int d = (int)Direction_Left; d < (int)Direction_Back + 1; d++)
			{
				center[(EDirection)d] = 0;
			}
			center[(EDirection)f] = 1024;
			m_NetworkNodes.AddNode(center, CSpeed(m_pUtil->GetUniformRandom(0, 20), m_pUtil->GetUniformRandom(-PI / 2, PI / 2)));
		}

		CNode::Sigma = 2 * 2;
	}
}

void CController::LoadNodesFromFile(std::string path)
{
	CNeuralNetwork newNetwork;

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

			newNetwork.AddNode(sensors, speed);
		}

		m_NetworkNodes = newNetwork;
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
	result.speed.Limit();
	m_pUtil->AddNetworkResult(result);

	// get value system's correction
	SIOSet ideal = m_pUtil->GetLastCorrectedResult();
	Adapt(ideal);

	//if (m_NetworkNodes.Count() != m_pSettings->MaxNodes()) RebuildNetwork();
}

void CController::Adapt(SIOSet ideal)
{
	auto current = m_NetworkNodes.Evaluate(ideal.sensors);

	m_NetworkNodes.Adapt(ideal);

	// output info
	std::ofstream file;
	file.open("History.txt", std::ios_base::app);

	bool babble = true;
	if (babble)
	{
		file << "ValueSystem's results:" << std::endl;
		ideal.sensors.Dump(file);
		file << " ==> Angle: " << (double)((int)(10 * ideal.speed.Angle())) / 10.0 << " Speed: " << round(ideal.speed.Velocity());
		file << std::endl;
	}
	if (babble)
	{
		CSpeed output = current.speed;
		output.Limit();
		file << "Controller's results:" << std::endl;
		ideal.sensors.Dump(file);
		file << " ==> Angle: " << (double)((int)(10 * output.Angle())) / 10.0 << " Speed: " << round(output.Velocity());
		file << std::endl;
	}

	file.close();
}

void CController::RebuildNetwork()
{
	CNeuralNetwork newNetwork;
	CNeuralNetwork oldNetwork = m_NetworkNodes;
	int nodeCount = m_pSettings->MaxNodes();

	for (int i = 0; i < nodeCount; i++)
	{
		Int8 raw = { {
				(int)round(m_pUtil->GetUniformRandom(0,1024)),
				(int)round(m_pUtil->GetUniformRandom(0,1024)),
				(int)round(m_pUtil->GetUniformRandom(0,1024)),
				(int)round(m_pUtil->GetUniformRandom(0,1024)),
				(int)round(m_pUtil->GetUniformRandom(0,1024)),
				(int)round(m_pUtil->GetUniformRandom(0,1024)),
				(int)round(m_pUtil->GetUniformRandom(0,1024)),
				(int)round(m_pUtil->GetUniformRandom(0,1024))
			} };
		CSensorData center = CSensorData(raw);
		CSpeed weight = oldNetwork.Evaluate(center).speed;

		newNetwork.AddNode(center, weight);
	}

	m_NetworkNodes = newNetwork;
}

SIOSet CController::Evaluate(CSensorData sensors)
{
	return m_NetworkNodes.Evaluate(sensors);
}

void CController::ListNodes()
{
    std::cout << "These are my nodes:" << std::endl;
    //for (auto it = m_NetworkNodes.begin(); it != m_NetworkNodes.end(); it++)
    //{
	//	it->Dump();
    //    std::cout << std::endl;
    //}
}
