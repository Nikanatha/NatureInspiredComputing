#include "ValueSystem.h"


CValueSystem::CValueSystem(CKheperaUtility * pUtil) : CThreadableBase(pUtil)
{
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
	SIOSet correction = history.front();
	CSensorData grad = history[1].sensors.GradientFrom(correction.sensors);

	std::map<CSpeed, double> speedFitness;

	// insert controller's results
	speedFitness[correction.speed] = Fitness(correction.sensors, grad, correction.speed);

	// generate alternatives
	CSpeed alt;
	
		// more left
	alt = correction.speed;
	alt.IncreaseAngle(m_pUtil->GetUniformRandom());
	speedFitness[alt] = Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt);

		// more right
	alt = correction.speed;
	alt.IncreaseAngle(-m_pUtil->GetUniformRandom());
	speedFitness[alt] = Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt);

		// more speed
	alt = correction.speed;
	alt *= 1 + m_pUtil->GetUniformRandom();
	speedFitness[alt] = Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt);

		// less speed
	alt = correction.speed;
	alt *= m_pUtil->GetUniformRandom();
	speedFitness[alt] = Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt);

	// choose best solution
	for (auto it = speedFitness.begin(); it != speedFitness.end(); it++)
	{
		if (it->second < speedFitness[correction.speed]) correction.speed = it->first;
	}

	return correction;
}

double CValueSystem::Fitness(CSensorData old, CSensorData change, CSpeed speed)
{
	// maximize velocity
	// minimize total change
	// weight directions in a way that front should not increase, neither should back (to prevent driving backwards)

	double speedPart;
	speedPart = exp(-speed.Velocity()); // expect values of up to 100


	double frontPart;
	frontPart = (change[Direction_FrontLeft].sensor + change[Direction_Front].sensor + change[Direction_FrontRight].sensor) * 2; // expect values of up to +- 3000

	double sidePart;
	sidePart = (change[Direction_Left].sensor + change[Direction_Right].sensor) * 1; // expect values of up to +- 2000

	double backPart;
	backPart = change[Direction_Back].sensor * 5; // expect alues of up to +- 1000

	return - speedPart + frontPart + sidePart + backPart;
}

CSensorData CValueSystem::PredictChange(CSensorData start, CSpeed speed)
{
	// very basic, naive assumptions:
		// linear (instead of exponential) sensor value increase for approaching
		// turning results in shifting the sensor values around

	CSensorData moved;
	CSensorData next;

	// straight component
	double straight = (speed.Left() + speed.Right()) / 2;

	// increases front and side front, decrease back
	EDirection dir;
	dir = Direction_FrontLeft;
	moved[dir] = SValue(start[dir].sensor + straight);
	dir = Direction_Front;
	moved[dir] = SValue(start[dir].sensor + straight);
	dir = Direction_FrontRight;
	moved[dir] = SValue(start[dir].sensor + straight);

	dir = Direction_Back;
	moved[dir] = SValue(start[dir].sensor - straight);

	dir = Direction_Left;
	moved[dir] = SValue(start[dir].sensor);
	dir = Direction_Right;
	moved[dir] = SValue(start[dir].sensor);

	// turning component
	EDirection turnDir;
	turnDir = AngleToDirection(speed.Angle());
	
	int limit = Direction_Back + 1;

	for (auto it = start.begin(); it != start.end(); it++)
	{
		EDirection dir = it->first;
		next[(EDirection)(((int)dir + limit + ((int)turnDir - (int)Direction_Front))%limit)] = SValue(it->second.sensor);
	}

	// return gradient
	return next.GradientFrom(start);
}
