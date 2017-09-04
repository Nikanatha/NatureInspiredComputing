#include "ValueSystem.h"


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
	alt.IncreaseAngle(m_pUtil->GetUniformRandom());
	speedFitness.push_back(FitSpeed(correction.sensors, alt));

	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(FitSpeed(correction.sensors, altBackwards));

		// more right
	alt = correction.speed;
	alt.IncreaseAngle(-m_pUtil->GetUniformRandom());
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
	// maximize velocity
	// minimize total change
	// weight directions in a way that front should not increase, neither should back (to prevent driving backwards)
	
	CSpeed limited = speed;
	limited.Limit();

	double speedPart = exp(5 * abs(speed.Velocity()) / abs(limited.Velocity()));
	if (speed.Velocity() < 0) speedPart *= 2;

	CSensorData future = PredictChange(position, speed);
	CSensorData farFuture = PredictChange(future, speed);

	double sensorPart = 0;
	for (int d = (int)Direction_Left; d <= (int)Direction_Back; d++)
	{
		EDirection dir = (EDirection)d;
		sensorPart += future[dir].sensor;
		sensorPart += farFuture[dir].sensor;
	}

	double fit = speedPart + sensorPart;

	bool reflex = false;
	if (farFuture.Collision()) reflex = true;
	//if (position[Direction_Front].proximity >= Proximity_Close && speed.Left() > 0 && speed.Right() > 0) reflex = true;
	
	if(reflex)
	{
		std::cout << "Reflex triggered at ";
		position.Dump();
		std::cout << std::endl;
		fit += 100000;
		m_Repetitions++;
		//Exception("Reflex triggered!", -1);
	}

	return fit;
}

SValue PredictValue(SValue start, double dirSpeed)
{
	// very basic assumption:
	// approaching occurs on a semi-linear curve, the faster the closer.
	double oldV = start.sensor;
	double expScale = exp(oldV - 1024);
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
