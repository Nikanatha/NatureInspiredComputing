#ifndef __Controller_H__
#define __Controller_H__

#include "ThreadedUtilityUser.h"
#include "Settings.h"
#include "SensorData.h"
#include "BraitenbergWeights.h"

class CController : public CThreadedUtilityUser
{
public:
	CController(CKheperaUtility* pUtil, CBraitenbergWeights* pWeights);

	void LoadFromFile(std::string path);
	void SaveToFile(std::string path);

protected:
	virtual void DoCycle();

private:
	SIOSet Evaluate(CSensorData sensors);

private:
	CBraitenbergWeights* m_pWeights;
};

#endif