#include <math.h>
#include <iostream>
#include <iomanip>
#include "Common.h"
#include "SensorData.h"

#define PROX_COLLISION_VALUE 1019

CSensorData::CSensorData(Int8 rawSensors)
{
	for (int i = 0; i < this->size(); i++) (*this)[i] = (double)rawSensors.data[i] / PROX_COLLISION_VALUE;
}

void CSensorData::Dump(std::ostream &stream)
{
	for (int d = 0; d < this->size(); d++)
	{
		stream << " " << std::setfill(' ') << std::setw(4) << this->at(d);
	}
}

int CSensorData::Collision()
{
	int collisionCount = 0;
	for (int d = 0; d < this->size(); d++)
	{
		if (this->at(d) > 1) collisionCount++;
	}
	return collisionCount;
}
