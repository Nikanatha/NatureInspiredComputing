#include <math.h>
#include <iostream>
#include <iomanip>
#include "Common.h"
#include "SensorData.h"

#define NEIGHBOR_WEIGHT 1
#define PROX_NEAR_VALUE 200
#define PROX_CLOSE_VALUE 500
#define PROX_COLLISION_VALUE 1000

CSensorData::CSensorData(Int8 rawSensors)
{
	(*this)[Direction_Left] = SValue(LeftSide(rawSensors));
	(*this)[Direction_FrontLeft] = SValue(LeftFront(rawSensors));
	(*this)[Direction_Front] = SValue(Front(rawSensors));
	(*this)[Direction_FrontRight] = SValue(RightFront(rawSensors));
	(*this)[Direction_Right] = SValue(RightSide(rawSensors));
	(*this)[Direction_Back] = SValue(Back(rawSensors));
}

void CSensorData::Dump()
{
	for (int d = 0; d <= (int)Direction_Back; d++)
	{
		std::cout << " " << std::setfill(' ') << std::setw(4) << this->at((EDirection)d).sensor;
	}
}

bool CSensorData::Collision()
{
	for (auto it = this->begin(); it != this->end(); it++)
	{
		if (it->second.proximity == Proximity_Collision) return true;
	}
	return false;
}

CSensorData CSensorData::GradientFrom(CSensorData previous)
{
	CSensorData gradient;
	for (auto it = this->begin(); it != this->end(); it++)
	{
		EDirection dir = it->first;
		gradient[dir] = SValue(it->second.sensor - previous[dir].sensor);
	}
	return gradient;
}

int CSensorData::Front(Int8 raw)
{
	double val = 0;

	val = fmax(val, raw.data[1] * NEIGHBOR_WEIGHT);
	val = fmax(val, raw.data[2]);
	val = fmax(val, raw.data[3]);
	val = fmax(val, raw.data[4] * NEIGHBOR_WEIGHT);

	return round(val);
}

int CSensorData::Back(Int8 raw)
{
	double val = 0;

	val = fmax(val, raw.data[6]);
	val = fmax(val, raw.data[7]);

	return round(val);
}

int CSensorData::LeftSide(Int8 raw)
{
	double val = 0;

	val = fmax(val, raw.data[0]);
	val = fmax(val, raw.data[1] * NEIGHBOR_WEIGHT);

	return round(val);
}

int CSensorData::LeftFront(Int8 raw)
{
	double val = 0;

	val = fmax(val, raw.data[0] * NEIGHBOR_WEIGHT);
	val = fmax(val, raw.data[1]);
	val = fmax(val, raw.data[2] * NEIGHBOR_WEIGHT);

	return round(val);
}

int CSensorData::RightSide(Int8 raw)
{
	double val = 0;

	val = fmax(val, raw.data[4] * NEIGHBOR_WEIGHT);
	val = fmax(val, raw.data[5]);

	return round(val);
}

int CSensorData::RightFront(Int8 raw)
{
	double val = 0;

	val = fmax(val, raw.data[3] * NEIGHBOR_WEIGHT);
	val = fmax(val, raw.data[4]);
	val = fmax(val, raw.data[5] * NEIGHBOR_WEIGHT);

	return round(val);
}


EProximity SValue::ProximityFromValue(double sensorVal)
{
	if (sensorVal > PROX_COLLISION_VALUE) return Proximity_Collision;
	if (sensorVal > PROX_CLOSE_VALUE) return Proximity_Close;
	if (sensorVal > PROX_NEAR_VALUE) return Proximity_Near;
	if (sensorVal > 0) return Proximity_Clear;
	return Proximity_Retreat;
}

EDirection AngleToDirection(double angle)
{
	bool positive = angle > 0;
	double absolute = abs(angle);
	
	if (absolute > 3 * PI / 4) return Direction_Back;
	if (absolute > 3 * PI / 12) return positive ? Direction_Left : Direction_Right;
	if (absolute > PI / 12) return positive ? Direction_FrontLeft : Direction_FrontRight;
	return Direction_Front;
}
