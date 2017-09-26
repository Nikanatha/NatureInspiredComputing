#include "BraitenbergWeights.h"

#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include <algorithm>

CBraitenbergWeights::CBraitenbergWeights()
{
	for (int i = 0; i < this->size() - 1; i++) (*this)[i].SetComponents(0, 0);
	(*this)[this->size() - 1].SetComponents(1, 1);

	LoadFromFile("default.txt");
}

void CBraitenbergWeights::Dump(std::ostream & stream)
{
	for (int d = 0; d < this->size(); d++)
	{
		stream << this->at(d).Left() << "," << this->at(d).Right() << ";";
	}
}

void CBraitenbergWeights::LoadFromFile(std::string path)
{
	std::fstream file;
	std::string line;

	file.open(path);
	if (file.is_open() && getline(file, line))
	{
		std::stringstream ss(line);
		std::string speed;
		std::array < CSpeed, 9> newWeights;
		int index = 0;
		while (index < newWeights.size() && getline(ss, speed, ';'))
		{
			std::stringstream sps(speed);
			std::string part;
			double left;
			double right;
			if (!getline(sps, part, ',')) return;
			left = std::stod(part);
			if (!getline(sps, part, ',')) return;
			right = std::stod(part);
			newWeights[index++].SetComponents(left, right);
		}
		if (index != newWeights.size()) return;

		for (int i = 0; i < this->size(); i++)
		{
			(*this)[i] = newWeights[i];
		}
	}
	else std::cout << "Unable to open file";

	file.close();
}

void CBraitenbergWeights::SaveToFile(std::string path)
{
	std::ofstream file;
	file.open(path, std::ios_base::trunc);

	if (file.is_open())
	{
		this->Dump(file);
		file << std::endl;
	}
	else std::cout << "Unable to open file";

	file.close();
}

void CBraitenbergWeights::VarySensorWeights(double sigma, std::function<double(double, double)> &rndFunc)
{
	std::cout << "Old weights: ";
	this->Dump();
	std::cout << std::endl;
	for (int i = 0; i < this->size(); i++)
	{
		double width = sigma*std::max(abs(this->at(i).Velocity()), abs(this->back().Velocity()));
		(*this)[i].SetComponents(
			rndFunc(width, this->at(i).Left()),
			rndFunc(width, this->at(i).Right())
		);
	}
	std::cout << "New weights: ";
	this->Dump();
	std::cout << std::endl;
}

void CBraitenbergWeights::MultiplySpeed(double factor)
{
	for (int i = 0; i < this->size(); i++) (*this)[i] *= factor;
}
