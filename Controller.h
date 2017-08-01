#ifndef __Controller_H__
#define __Controller_H__

#include "ThreadableBase.h"
#include "Settings.h"
#include "SensorData.h"
#include "Node.h"

class CController : public CThreadableBase
{
public:
	CController(CKheperaUtility* pUtil, CRbfSettings* pSettings);

	void LoadNodesFromFile(std::string path);
	void SaveNodesToFile(std::string path);
    void ListNodes();

protected:
	virtual void DoCycle();

private:
	SIOSet Evaluate(CSensorData sensors);
	void Adapt(SIOSet ideal);

	// rbf network functions
	void AddNode(CSensorData, CSpeed speed);
	void Forget();

private:
	CRbfSettings* m_pSettings;

	std::vector<CNode> m_NetworkNodes;
};

#endif