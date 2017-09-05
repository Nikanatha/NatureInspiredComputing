#ifndef __SmartKhepera_H__
#define __SmartKhepera_H__

#include <string>

#include "Settings.h"
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
    
    void StopMoving();

	void StartVerbosity();
	void StopVerbosity();

	void OpenSettingsMenu();

	void SaveNodes(std::string path);
	void LoadNodes(std::string path);

private:
	CRbfSettings* m_pRbfSettings;

	CKheperaUtility* m_pUtil;
	CController* m_pControl;
	CValueSystem* m_pValues;
	COperator* m_pOperate;
};

#endif