#include <math.h>
#include <iostream>
#include <algorithm>

#include "Node.h"

double CNode::Sigma = 1;
double CNode::LearningWeight = 0.3;
const double CNode::DecayRate = 0.9;

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
	m_Weight.Limit();
}

void CNode::Adapt(CSensorData input, CSpeed old, CSpeed better, double totalActivity)
{
	double activation;
	CSpeed output;
	activation = Calculate(input, output);
	
	double strength = LearningWeight * activation;
	
	if (activation > 0.5)
	{
		std::cout << "Node activity = " << activation << ", Old L R = " << m_Weight.Left() << " " << m_Weight.Right() << ", ";
	}

	CSpeed change = (better - old) * LearningWeight;
	m_Weight += change;
	
	if (activation > 0.5)
	{
		std::cout << "New L R = " << m_Weight.Left() << " " << m_Weight.Right() << std::endl;
	}

	m_Weight.Limit();
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
		double diff = sens->second.sensor - m_Center[sens->first].sensor;
		sqdist += pow(diff/1024.0, 2);
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


void CNeuralNetwork::AddNode(CSensorData sensors, CSpeed speed)
{
	CNode node(sensors, speed);

	this->push_back(node);
	std::cout << "Added new ";
	node.Dump();
	std::cout << std::endl;

}

void CNeuralNetwork::Forget(int maxCount)
{
	std::sort(this->begin(), this->end(), CNode::CompareActivity);

	int count = 0;
	for (auto it = this->begin(); it != this->end(); it++)
	{
		count++;
		if (count > maxCount)
		{
			std::cout << "Forgetting ";
			it->Dump();
			std::cout << std::endl;
		}
	}

	this->resize(maxCount);
}

int CNeuralNetwork::Count()
{
	return this->size();
}

SIOSet CNeuralNetwork::Evaluate(CSensorData sensors)
{
	double activation = 0;
	double maxAct = 0;
	CSpeed speed;

	for (int n = 0; n < this->size(); n++)
	{
		double act;
		CSpeed out;
		act = this->at(n).Activate(sensors, out);
		speed += out;
		activation += act;
		maxAct = fmax(maxAct, act);
	}

	if (activation > 0) speed /= activation;

	SIOSet result;
	result.sensors = sensors;
	result.speed = speed;

//	if (activation>1.2) printf("Activation is too high (%f)\n", activation);
	if (activation<0.8) printf("Activation is too low (%f)\n", activation);
	return result;
}

void CNeuralNetwork::Adapt(SIOSet ideal)
{
	double activation = 0;
	CSpeed current;

	for (int n = 0; n < this->size(); n++)
	{
		double act;
		CSpeed out;
		act = this->at(n).Calculate(ideal.sensors, out);
		current += out;
		activation += act;
	}

	if (activation > 0)	current /= activation;
	ideal.speed.Limit();

	for (int n = 0; n < this->size(); n++)
	{
		this->at(n).Adapt(ideal.sensors, current, ideal.speed, activation);
	}
}
