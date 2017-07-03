#include <math.h>
#include "Controller.h"

#define DISTANCE_LIMIT (0.9 * MIN_SENSOR_VAL)

CController::CController(CKheperaUtility * pUtil) : CThreadableBase(pUtil)
{
	m_Sigma = 1;
	m_LearnWeight = 0.3;

	// generate nodes for network
	for (int n = 0; n < NODE_COUNT; n++)
	{
		Int8 center;
		for (int i = 0; i < INPUT_COUNT; i++)
		{
			center.data[i] = round(m_pUtil->GetUniformRandom(MAX_SENSOR_VAL, MIN_SENSOR_VAL));
		}

		SNode node;
		node.center = center;
		node.lWeight = m_pUtil->GetUniformRandom(MAX_SPEED, -MAX_SPEED);
		node.rWeight = m_pUtil->GetUniformRandom(MAX_SPEED, -MAX_SPEED);
		m_NetworkNodes.push_back(node);
	}

	// pre-train
	CreateTrainingData();
	Train();
}

void CController::DoCycle()
{
	// evaluate current sensor data
	Int8 sensorData = m_pUtil->GetSensorData();
	SIOSet result = Evaluate(sensorData);
	m_pUtil->SetNetworkResult(result);

	// get value system's correction
	SIOSet ideal = m_pUtil->GetLastCorrectedResult();
	Adapt(ideal);
}

void CController::Adapt(SIOSet ideal)
{
	SIOSet current = Evaluate(ideal.sensors);

	for (int n = 0; n < m_NetworkNodes.size(); n++)
	{
		double act = RbfBase(ideal.sensors, m_NetworkNodes[n].center);
		m_NetworkNodes[n].lWeight += (ideal.speed.left - current.speed.left) * act * m_LearnWeight;
		m_NetworkNodes[n].rWeight += (ideal.speed.right - current.speed.right) * act * m_LearnWeight;
	}
}

double CController::RbfBase(Int8 sensors, Int8 nodeCenter)
{
	double sqdist = 0;
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		double diff = (sensors.data[i] - nodeCenter.data[i]) / SENSOR_VAL_RANGE;
		sqdist += pow(diff, 2);
	}
	return exp(-sqrt(sqdist) / m_Sigma);
}

void CController::CreateTrainingData()
{
	m_TrainingData.clear();

	Int8 sensors;

	// free field
	sensors.data[0] = MAX_SENSOR_VAL;
	sensors.data[1] = MAX_SENSOR_VAL;
	sensors.data[2] = MAX_SENSOR_VAL;
	sensors.data[3] = MAX_SENSOR_VAL;
	sensors.data[4] = MAX_SENSOR_VAL;
	sensors.data[5] = MAX_SENSOR_VAL;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(MAX_SPEED, MAX_SPEED)));

	// corridor
	sensors.data[0] = DISTANCE_LIMIT;
	sensors.data[1] = 0.5*DISTANCE_LIMIT;
	sensors.data[2] = MAX_SENSOR_VAL;
	sensors.data[3] = MAX_SENSOR_VAL;
	sensors.data[4] = 0.5*DISTANCE_LIMIT;
	sensors.data[5] = DISTANCE_LIMIT;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(MAX_SPEED, MAX_SPEED)));

	// back to dead end
	sensors.data[0] = DISTANCE_LIMIT;
	sensors.data[1] = MAX_SENSOR_VAL;
	sensors.data[2] = MAX_SENSOR_VAL;
	sensors.data[3] = MAX_SENSOR_VAL;
	sensors.data[4] = MAX_SENSOR_VAL;
	sensors.data[5] = DISTANCE_LIMIT;
	sensors.data[6] = DISTANCE_LIMIT;
	sensors.data[7] = DISTANCE_LIMIT;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(MAX_SPEED, MAX_SPEED)));


	// soft left corner
	sensors.data[0] = DISTANCE_LIMIT;
	sensors.data[1] = DISTANCE_LIMIT;
	sensors.data[2] = MAX_SENSOR_VAL;
	sensors.data[3] = MAX_SENSOR_VAL;
	sensors.data[4] = MAX_SENSOR_VAL;
	sensors.data[5] = MAX_SENSOR_VAL;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(MAX_SPEED, 0.5*MAX_SPEED)));

	// left corner
	sensors.data[0] = DISTANCE_LIMIT;
	sensors.data[1] = DISTANCE_LIMIT;
	sensors.data[2] = DISTANCE_LIMIT;
	sensors.data[3] = MAX_SENSOR_VAL;
	sensors.data[4] = MAX_SENSOR_VAL;
	sensors.data[5] = MAX_SENSOR_VAL;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(MAX_SPEED, -MAX_SPEED)));

	// frontal left
	sensors.data[0] = MAX_SENSOR_VAL;
	sensors.data[1] = MAX_SENSOR_VAL;
	sensors.data[2] = DISTANCE_LIMIT;
	sensors.data[3] = DISTANCE_LIMIT;
	sensors.data[4] = MAX_SENSOR_VAL;
	sensors.data[5] = MAX_SENSOR_VAL;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(-0.5*MAX_SPEED, -MAX_SPEED)));

	// frontal right
	sensors.data[0] = MAX_SENSOR_VAL;
	sensors.data[1] = MAX_SENSOR_VAL;
	sensors.data[2] = MAX_SENSOR_VAL;
	sensors.data[3] = DISTANCE_LIMIT;
	sensors.data[4] = DISTANCE_LIMIT;
	sensors.data[5] = MAX_SENSOR_VAL;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(-MAX_SPEED, -0.5*MAX_SPEED)));

	// soft right corner
	sensors.data[0] = MAX_SENSOR_VAL;
	sensors.data[1] = MAX_SENSOR_VAL;
	sensors.data[2] = MAX_SENSOR_VAL;
	sensors.data[3] = MAX_SENSOR_VAL;
	sensors.data[4] = DISTANCE_LIMIT;
	sensors.data[5] = DISTANCE_LIMIT;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(0.5*MAX_SPEED, MAX_SPEED)));

	// right corner
	sensors.data[0] = MAX_SENSOR_VAL;
	sensors.data[1] = MAX_SENSOR_VAL;
	sensors.data[2] = MAX_SENSOR_VAL;
	sensors.data[3] = DISTANCE_LIMIT;
	sensors.data[4] = DISTANCE_LIMIT;
	sensors.data[5] = DISTANCE_LIMIT;
	sensors.data[6] = MAX_SENSOR_VAL;
	sensors.data[7] = MAX_SENSOR_VAL;
	m_TrainingData.push_back(SIOSet(sensors, SSpeed(-MAX_SPEED, MAX_SPEED)));
}

void CController::Train()
{
	for (int i = 0; i < 10 * m_TrainingData.size(); i++)
	{
		Adapt(m_TrainingData[i%m_TrainingData.size()]);
	}
}

SIOSet CController::Evaluate(Int8 sensors)
{
	SIOSet out;
	out.sensors = sensors;
	double totalActivation = 0;

	for (int n = 0; n < m_NetworkNodes.size(); n++)
	{
		SNode node = m_NetworkNodes[n];
		double act = RbfBase(sensors, node.center);
		out.speed.left += act * node.lWeight;
		out.speed.right += act * node.rWeight;
		totalActivation += act;
	}

	out.speed.left /= totalActivation;
	out.speed.right /= totalActivation;
	return out;
}
