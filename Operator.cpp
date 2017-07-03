#include <math.h>

#include "Operator.h"

COperator::COperator(CKheperaUtility * pUtil) : CThreadableBase(pUtil)
{

}

void COperator::DoCycle()
{
	// convert target speed to noisy int for khepera
	SSpeed target = m_pUtil->GetLastNetworkResult().speed;
	Int2 rounded = NoiseRound(target);
	m_pUtil->SetSpeed(rounded);
}

Int2 COperator::NoiseRound(SSpeed target)
{
	Int2 rounded;
	rounded.data[0] = round(target.left + m_pUtil->GetUniformRandom(0.5, -0.5));
	rounded.data[1] = round(target.right + m_pUtil->GetUniformRandom(0.5, -0.5));
	return rounded;
}

