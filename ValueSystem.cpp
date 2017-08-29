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
	SIOSet correction = history.front();
	CSensorData next = PredictChange(correction.sensors, correction.speed);

	// if repetitions are required, just output the last speed
	if (m_Repetitions > 0)
	{
		m_Repetitions--;
		correction.speed = m_RepeatSpeed;
		speedFitness.push_back(std::make_pair(m_RepeatSpeed, Fitness(correction.sensors, PredictChange(correction.sensors, correction.speed), m_RepeatSpeed)));
	}

	// insert controller's results
	speedFitness.push_back(std::make_pair(correction.speed, Fitness(correction.sensors, next, correction.speed)));

	// generate alternatives
	CSpeed alt;
	CSpeed altBackwards;

		// backwards
	altBackwards = correction.speed;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(std::make_pair(altBackwards, Fitness(correction.sensors,
		PredictChange(correction.sensors, altBackwards),
		altBackwards)));

		// more left
	alt = correction.speed;
	alt.IncreaseAngle(m_pUtil->GetUniformRandom());
	speedFitness.push_back(std::make_pair(alt, Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt)));
	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(std::make_pair(altBackwards, Fitness(correction.sensors,
		PredictChange(correction.sensors, altBackwards),
		altBackwards)));

		// more right
	alt = correction.speed;
	alt.IncreaseAngle(-m_pUtil->GetUniformRandom());
	speedFitness.push_back(std::make_pair(alt, Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt)));
	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(std::make_pair(altBackwards, Fitness(correction.sensors,
		PredictChange(correction.sensors, altBackwards),
		altBackwards)));

	
		// more speed
	alt = correction.speed;
	alt *= 1 + m_pUtil->GetUniformRandom();
	speedFitness.push_back(std::make_pair(alt, Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt)));
	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(std::make_pair(altBackwards, Fitness(correction.sensors,
		PredictChange(correction.sensors, altBackwards),
		altBackwards)));

		// less speed
	alt = correction.speed;
	alt *= m_pUtil->GetUniformRandom();
	speedFitness.push_back(std::make_pair(alt, Fitness(correction.sensors,
		PredictChange(correction.sensors, alt),
		alt)));
	altBackwards = alt;
	altBackwards.SetVelocity(-altBackwards.Velocity());
	speedFitness.push_back(std::make_pair(altBackwards, Fitness(correction.sensors,
		PredictChange(correction.sensors, altBackwards),
		altBackwards)));
		
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

double CValueSystem::Fitness(CSensorData old, CSensorData change, CSpeed speed)
{
	// maximize velocity
	// minimize total change
	// weight directions in a way that front should not increase, neither should back (to prevent driving backwards)
	
	double speedPart;
	if (speed.Velocity() < 0) speedPart = 0;
	else speedPart = (speed.Velocity()-50)*10; //exp(-speed.Velocity()); // expect values of up to 100


	double frontPart;
	frontPart = (change[Direction_FrontLeft].sensor + change[Direction_Front].sensor + change[Direction_FrontRight].sensor) * 2; // expect values of up to +- 3000

	double sidePart;
	sidePart = (change[Direction_Left].sensor + change[Direction_Right].sensor) * 1; // expect values of up to +- 2000

	double backPart;
	backPart = change[Direction_Back].sensor * 5; // expect alues of up to +- 1000

	double fit = speedPart + frontPart + sidePart + backPart;

	bool reflex = false;

	if (change.Collision()) reflex = true;
	if (old[Direction_Front].proximity >= Proximity_Close && speed.Left() > 0 && speed.Right() > 0) reflex = true;
	
	if(reflex)
	{
		std::cout << "Reflex triggered at ";
		old.Dump();
		std::cout << std::endl;
		fit += 10000;
		m_Repetitions++;
		//Exception("Reflex triggered!", -1);
	}

	return fit;
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

	for (auto it = moved.begin(); it != moved.end(); it++)
	{
		EDirection dir = it->first;
		EDirection newDir = (EDirection)(((int)dir + limit + ((int)turnDir - (int)Direction_Front)) % limit);
//		next[newDir] = SValue((it->second.sensor + moved[newDir].sensor) / 2);
		next[newDir] = SValue(fmin(fmax(0, it->second.sensor), 1024));
	}

	// return gradient
	return next;//.GradientFrom(start);
}

std::pair<CSpeed, double> CValueSystem::FitSpeed(CSensorData start, CSpeed speed)
{
	double fit;

	try
	{
	fit = Fitness(start,
		PredictChange(start, speed),
		speed);
	}
	catch(...)
	{
		m_Repetitions = 3;
		return std::make_pair(CSpeed(0,0), 100000);
	}

	return std::make_pair(speed, fit);
}
