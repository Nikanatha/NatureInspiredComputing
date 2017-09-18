#include <math.h>
#include <iostream>
#include <iomanip>
#include "Common.h"
#include "SensorData.h"

#define PROX_COLLISION_VALUE 1000

CSensorData::CSensorData(Int8 rawSensors)
{
	for (int i = 0; i < this->size(); i++) (*this)[i] = rawSensors.data[i];
}

void CSensorData::Dump(std::ostream &stream)
{
	for (int d = 0; d < this->size(); d++)
	{
		stream << " " << std::setfill(' ') << std::setw(4) << this->at(d);
	}
}

bool CSensorData::Collision()
{
	for (int d = 0; d < this->size(); d++)
	{
		if (this->at(d) > PROX_COLLISION_VALUE) return true;
	}
	return false;
}
