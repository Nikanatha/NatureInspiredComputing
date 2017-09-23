#ifndef __SensorData_H__
#define __SensorData_H__

#include <array>
#include <iostream>
#include "KheperaDefines.h"

typedef double SensorValue;

class CSensorData : public std::array<SensorValue, 8>
{
public:
	CSensorData(Int8 rawSensors = { {0, 0, 0, 0, 0, 0, 0, 0} });
	void Dump(std::ostream &stream = std::cout);
	int Collision();
};

#endif