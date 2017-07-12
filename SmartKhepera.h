#ifndef __SmartKhepera_H__
#define __SmartKhepera_H__

#include "KheperaUtility.h"
#include "Controller.h"
#include "ValueSystem.h"
#include "Operator.h"

class CSmartKhepera
{
public:
	CSmartKhepera();
	~CSmartKhepera();

	void StartRobot();
	void StopRobot();
	void StartLearning();
	void StopLearning();

private:
	CKheperaUtility* m_pUtil;
	CController* m_pControl;
	CValueSystem* m_pValues;
	COperator* m_pOperate;
};

#endif