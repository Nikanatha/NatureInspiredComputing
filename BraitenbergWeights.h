#ifndef __BraitenbergWeights_H__
#define __BraitenbergWeights_H__

#include <array>
#include <iostream>
#include <functional>
#include "Speed.h"

class CBraitenbergWeights : public std::array<CSpeed, 8+1>
{
public:
	CBraitenbergWeights();

	void Dump(std::ostream &stream = std::cout);
	void LoadFromFile(std::string path);
	void SaveToFile(std::string path);

	void VarySensorWeights(double sigma, std::function<double(double, double)> &rndFunc);
	void MultiplySpeed(double factor);
};

#endif