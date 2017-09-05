#include "ValueSystem.h"
#include <fstream>

CValueSystem::CValueSystem(CKheperaUtility * pUtil) : CThreadableBase(pUtil)
{
	m_Repetitions = 0;
}

void CValueSystem::DoCycle()
{
	// evaluate current results
	std::vector<SIOSet> controllerResults = m_pUtil->GetNetworkResults();
	if (controllerResults.size() < 2) return;
	SIOSet correction = Correct(controllerResults);
	m_pUtil->SetCorrectedResult(correction);
}

SIOSet CValueSystem::Correct(std::vector<SIOSet> history)
{
	std::vector<std::pair<CSpeed, double>> speedFitness;
	SIOSet correction = history.back();

	// if repetitions are required, just output the last speed
	if (m_Repetitions > 0)
	{
		m_Repetitions--;
		correction.speed = m_RepeatSpeed;
		speedFitness.push_back(FitSpeed(correction.sensors, m_RepeatSpeed));
	}

	// insert controller's results
	speedFitness.push_back(FitSpeed(correction.sensors, correction.speed));

	// generate alternatives
	CSpeed alt;
	CSpeed altBackwards;

		// backwards
	altBackwards = correction.speed;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(FitSpeed(correction.sensors, altBackwards));

		// more left
	alt = correction.speed;
	alt.IncreaseAngle(PI/4);
	speedFitness.push_back(FitSpeed(correction.sensors, alt));

	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(FitSpeed(correction.sensors, altBackwards));

		// more right
	alt = correction.speed;
	alt.IncreaseAngle(-PI/4);
	speedFitness.push_back(FitSpeed(correction.sensors, alt));

	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(FitSpeed(correction.sensors, altBackwards));

	
		// more speed
	alt = correction.speed;
	alt *= 1 + m_pUtil->GetUniformRandom();
	speedFitness.push_back(FitSpeed(correction.sensors, alt));

	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(FitSpeed(correction.sensors, altBackwards));

		// less speed
	alt = correction.speed;
	alt *= m_pUtil->GetUniformRandom();
	speedFitness.push_back(FitSpeed(correction.sensors, alt));

	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(FitSpeed(correction.sensors, altBackwards));
		
	// choose best solution
	std::pair<CSpeed, double> best = speedFitness.front();
	for (auto it = speedFitness.begin(); it != speedFitness.end(); it++)
	{
		if (it->second < best.second) best = *it;
	}

	correction.speed = best.first;
	m_RepeatSpeed = correction.speed;
	return correction;
}

double CValueSystem::Fitness(CSensorData position, CSpeed speed)
{
	double fit = 0;

	int steps = 3;
	CSensorData older = position;
	while (steps > 0)
	{
		CSensorData future = PredictChange(older, speed);
		fit += SensorFitness(future);
		older = future;
		steps--;
	}
	fit /= 3;

	fit += 5*SpeedFitness(speed);

	// output info
	std::ofstream file;
	file.open("Fitness.txt", std::ios_base::app);

	bool babble = true;
	if (babble)
	{
		position.Dump(file);
		file << " ==> Angle: " << (double)((int)(100 * speed.Angle())) / 100.0 << " Speed: " << (double)((int)(100 * speed.Velocity())) / 100.0;
		file << " => Fitness: " << fit;
		file << std::endl;
	}

	return fit;
}

double CValueSystem::SpeedFitness(CSpeed speed)
{
	double V = exp(-abs(speed.Velocity()));
	double A = abs(speed.Angle());
	double backPenalty = -speed.Velocity() / abs(speed.Velocity());
	return V + 1.2*A + backPenalty;
}

double CValueSystem::SensorFitness(CSensorData sensors)
{
	double fit = 0;

	double penalty = 10 * 1024;
	if (sensors.Collision())
	{
		if (sensors[Direction_Back].proximity == Proximity_Collision)
			fit += penalty / 2;
		else
			fit += penalty;
	}

	fit += 1 * sensors[Direction_Left].sensor;
	fit += 1.2 * sensors[Direction_FrontLeft].sensor;
	fit += 1.3 * sensors[Direction_Front].sensor;
	fit += 1.2 * sensors[Direction_FrontRight].sensor;
	fit += 1 * sensors[Direction_Right].sensor;
	fit += 5 * sensors[Direction_Back].sensor;

	return fit;
}

SValue PredictValue(SValue start, double dirSpeed)
{
	// very basic assumption:
	// approaching occurs on a semi-linear curve, the faster the closer.
	double oldV = start.sensor;
	double expScale = 10*exp(oldV - 1024);
	double newV = round(oldV + expScale*dirSpeed);
	if (newV == oldV)
	{
		if (dirSpeed > 0) newV++;
		if (dirSpeed < 0) newV--;
	}
	if (newV < 0) newV = 0;
	return SValue((int)newV);
}

CSensorData CValueSystem::PredictChange(CSensorData start, CSpeed speed)
{
	CSensorData next;

	// straight component
	double straight = (speed.Left() + speed.Right()) / 2;

	// increases front and side front, decrease back
	EDirection dir;
	dir = Direction_Left;
	next[dir] = PredictValue(start[dir], speed.Left());
	dir = Direction_FrontLeft;
	next[dir] = PredictValue(start[dir], (speed.Left() + straight) / 2);
	dir = Direction_Front;
	next[dir] = PredictValue(start[dir], straight);
	dir = Direction_FrontRight;
	next[dir] = PredictValue(start[dir], (speed.Right() + straight) / 2);
	dir = Direction_Right;
	next[dir] = PredictValue(start[dir], speed.Right());

	dir = Direction_Back;
	next[dir] = PredictValue(start[dir], - straight);

	return next;
}

std::pair<CSpeed, double> CValueSystem::FitSpeed(CSensorData start, CSpeed speed)
{
	double fit;

	try
	{
		fit = Fitness(start, speed);
	}
	catch(...)
	{
		m_Repetitions = 3;
		return std::make_pair(CSpeed(0,0), 100000);
	}

	return std::make_pair(speed, fit);
}
