#include <math.h>
#include <iostream>

#include "Node.h"

double CNode::Sigma = 10;
double CNode::LearningWeight = 0.3;
const double CNode::DecayRate = 0.9;

#define DIFF_FACTOR 0.05

CNode::CNode()
{
	m_Center = CSensorData();
	m_Weight = CSpeed();
	m_Activity = 0;
}

CNode::CNode(CSensorData c, CSpeed w) : CNode()
{
	m_Center = c;
	m_Weight = w;
	m_Weight.Limit();
	AddActivation();
}

double CNode::Activate(CSensorData input, CSpeed & output)
{
	double activation;
	Decay();
	activation = Calculate(input, output);
	AddActivation(activation);
	return activation;
}

double CNode::Calculate(CSensorData input, CSpeed & output)
{
	m_Weight.Limit();

	double activation;
	activation = BaseFunction(input);
	output = m_Weight * activation;
	return activation;
}

void CNode::Adapt(CSensorData input, CSpeed difference)
{
	double activation;
	CSpeed output;
	activation = Calculate(input, output);

	CSpeed change = difference * activation * LearningWeight;
	m_Weight += change;
}

void CNode::Adapt(CSensorData input, CSpeed old, CSpeed better)
{
	double activation;
	CSpeed output;
	activation = Calculate(input, output);

	double vChange = (better.Velocity() - old.Velocity()) * activation * LearningWeight;
	double aChange = (better.Angle() - old.Angle()) * activation * LearningWeight;
	m_Weight.IncreaseVelocity(vChange);
	m_Weight.IncreaseAngle(aChange);
}

CSensorData CNode::Center()
{
	return m_Center;
}

CSpeed CNode::Weight()
{
	return m_Weight;
}

void CNode::Dump()
{
	std::cout << "Node (";
	m_Center.Dump();
	std::cout << " ) : W = " << m_Weight.Velocity() << "(" << m_Weight.Angle() << ")" << "; Act = " << m_Activity;
}

bool CNode::CompareActivity(CNode a, CNode b)
{
	return a.m_Activity > b.m_Activity;
}

double CNode::BaseFunction(CSensorData sensors)
{
	double sqdist = 0;

	for (auto sens = sensors.begin(); sens != sensors.end(); sens++)
	{
		int diff = sens->second.sensor - m_Center[sens->first].sensor;
		sqdist += pow(DIFF_FACTOR*diff, 2); // TODO: replae explicit 1000
	}

	return exp(-sqrt(sqdist) / Sigma);
}

void CNode::Decay()
{
	m_Activity *= DecayRate;
}

void CNode::AddActivation(double act)
{
	m_Activity += act;
}
