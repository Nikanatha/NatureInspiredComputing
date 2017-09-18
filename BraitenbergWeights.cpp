#include "BraitenbergWeights.h"

#include <iomanip>

CBraitenbergWeights::CBraitenbergWeights()
{
	for (int i = 0; i < this->size() - 1; i++) (*this)[i].SetComponents(0, 0);
	(*this)[this->size() - 1].SetComponents(1, 1);
}

void CBraitenbergWeights::Dump(std::ostream & stream)
{
	for (int d = 0; d < this->size(); d++)
	{
		stream << this->at(d).Left() << "," << this->at(d).Right() << ";";
	}
}

void CBraitenbergWeights::VarySensorWeights(double sigma, std::function<double(double, double)> &rndFunc)
{
	for (int i = 0; i < this->size() - 1; i++) (*this)[i].SetComponents(
		rndFunc(sigma, this->at(i).Left()),
		rndFunc(sigma, this->at(i).Right())
		);
}

void CBraitenbergWeights::MultiplySpeed(double factor)
{
	for (int i = 0; i < this->size(); i++) (*this)[i] *= factor;
}
