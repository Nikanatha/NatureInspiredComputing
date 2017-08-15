#ifndef __SensorData_H__
#define __SensorData_H__

#include <map>
#include <iostream>
#include "KheperaInterface.h"

enum EDirection
{
	Direction_Left = 0,
	Direction_FrontLeft = 1,
	Direction_Front = 2,
	Direction_FrontRight = 3,
	Direction_Right = 4,
	Direction_Back = 5
};

EDirection AngleToDirection(double angle);

enum EProximity
{
	Proximity_Retreat = -1,
	Proximity_Clear = 0,
	Proximity_Near = 1,
	Proximity_Close = 2,
	Proximity_Collision = 3
};

struct SValue
{
	int sensor;
	EProximity proximity;
	SValue() { sensor = 0; proximity = ProximityFromValue(0); }
	SValue(int s) { sensor = s; proximity = ProximityFromValue(s); }

private:
	EProximity ProximityFromValue(double sensorVal);
};

class CSensorData : public std::map<EDirection, SValue>
{
public:
	CSensorData(Int8 rawSensors = { {0, 0, 0, 0, 0, 0, 0, 0} });
	void Dump(std::ostream &stream = std::cout);
	bool Collision();

	CSensorData GradientFrom(CSensorData previous);

private:
	int Front(Int8 raw);
	int Back(Int8 raw);

	int LeftSide(Int8 raw);
	int LeftFront(Int8 raw);

	int RightSide(Int8 raw);
	int RightFront(Int8 raw);
};

#endif