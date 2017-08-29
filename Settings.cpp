#include "Settings.h"

#include "Node.h"
#include <math.h>

CRbfSettings::CRbfSettings()
{
	SetMaxNodes(10);
}

double CRbfSettings::Sigma()
{
	return CNode::Sigma;
}

double CRbfSettings::Learn()
{
	return CNode::LearningWeight;
}

void CRbfSettings::SetLearn(double val)
{
	CNode::LearningWeight = val;
}

int CRbfSettings::MaxNodes()
{
	return m_MaxNodes;
}

void CRbfSettings::SetMaxNodes(int val)
{
	m_MaxNodes = val;
	CNode::Sigma = (-1024.0 / (2 * m_MaxNodes*log(0.5)));
}

CMscSettings::CMscSettings()
{

}
