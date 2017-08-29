#ifndef __ValueSystem_H__
#define __ValueSystem_H__

#include "ThreadableBase.h"

class CValueSystem : public CThreadableBase
{
public:
	CValueSystem(CKheperaUtility* pUtil);

protected:
	virtual void DoCycle();

private:
	SIOSet Correct(std::vector<SIOSet> history);
	double Fitness(CSensorData old, CSensorData change, CSpeed speed);
	CSensorData PredictChange(CSensorData start, CSpeed speed);

	std::pair<CSpeed, double> FitSpeed(CSensorData start, CSpeed speed);

private:
	int m_Repetitions;
	CSpeed m_RepeatSpeed;
};

#endif