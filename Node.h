#ifndef __Node_H__
#define __Node_H__

#include "SensorData.h"
#include "Speed.h"

class CNode
{
public:
	static double Sigma;
	static double LearningWeight;

public:
	CNode();
	CNode(CSensorData c, CSpeed w);

	double Activate(CSensorData input, CSpeed& output);
	double Calculate(CSensorData input, CSpeed& output);
	void Adapt(CSensorData input, CSpeed difference);
	void Adapt(CSensorData input, CSpeed old, CSpeed better, double totalActivity);

	CSensorData Center();
	CSpeed Weight();
	void Dump();

	static bool CompareActivity(CNode a, CNode b);

private:
	double BaseFunction(CSensorData sensors);
	void Decay();
	void AddActivation(double act = 1);

private:
	static const double DecayRate;

	CSensorData m_Center;
	CSpeed m_Weight;
	double m_Activity;
};

#endif