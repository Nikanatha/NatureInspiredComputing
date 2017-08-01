#ifndef __SensorData_H__
#define __SensorData_H__

#include <map>
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
	Proximity_Clear = 0,
	Proximity_Near = 1,
	Proximity_Close = 2,
	Proximity_Collision = 3
};

class CSensorData : public std::map<EDirection, EProximity>
{
public:
	CSensorData(Int8 rawSensors = { {0, 0, 0, 0, 0, 0, 0, 0} });
	void Dump();
	bool Collision();

private:
	EProximity Front(Int8 raw);
	EProximity Back(Int8 raw);

	EProximity LeftSide(Int8 raw);
	EProximity LeftFront(Int8 raw);

	EProximity RightSide(Int8 raw);
	EProximity RightFront(Int8 raw);
	

	EProximity FromValue(double sensorVal);
};

#endif