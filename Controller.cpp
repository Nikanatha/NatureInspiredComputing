#include <fstream>
#include <sstream>
#include <string>

#include <algorithm>
#include <iostream>
#include "Controller.h"

CController::CController(CKheperaUtility * pUtil, CBraitenbergWeights* pWeights) : CThreadedUtilityUser(pUtil)
{
	m_pWeights = pWeights;

	LoadFromFile("best.txt");
}

void CController::LoadFromFile(std::string path)
{
	std::fstream file;
	std::string line;

	file.open(path);
	if (file.is_open() && getline(file, line))
	{
		std::stringstream ss(line);
		std::string speed;
		std::array < CSpeed, 9> newWeights;
		int index = 0;
		while (index < newWeights.size() && getline(ss, speed, ';'))
		{
			std::stringstream sps(speed);
			std::string part;
			double left;
			double right;
			if (!getline(sps, part, ',')) return;
			left = std::stod(part);
			if (!getline(sps, part, ',')) return;
			right = std::stod(part);
			newWeights[index++].SetComponents(left, right);
		}
		if (index != newWeights.size()) return;

		for (int i = 0; i < m_pWeights->size(); i++)
		{
			(*m_pWeights)[i] = newWeights[i];
		}
	}
	else std::cout << "Unable to open file";

	file.close();
}

void CController::SaveToFile(std::string path)
{
	std::ofstream file;
	file.open(path, std::ios_base::trunc);

	if (file.is_open())
	{
		m_pWeights->Dump(file);
		file << std::endl;
	}
	else std::cout << "Unable to open file";

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
}

SIOSet CController::Evaluate(CSensorData sensors)
{
	CSpeed result;
	for (int i = 0; i < m_pWeights->size() - 1; i++)
	{
		result += m_pWeights->at(i) * sensors[i];
	}
	result += m_pWeights->back();

	SIOSet out;
	out.sensors = sensors;
	out.speed = result;
	return out;
}
