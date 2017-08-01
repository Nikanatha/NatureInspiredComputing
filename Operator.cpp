#include <math.h>

#include "Operator.h"

COperator::COperator(CKheperaUtility * pUtil) : CThreadableBase(pUtil)
{

}

void COperator::DoCycle()
{
	// convert target speed to noisy int for khepera
	CSpeed target =m_pUtil->GetLastNetworkResult().speed;
	CSpeed noisy = Noisy(target);
	m_pUtil->SetSpeed(round(noisy.Left()), round(noisy.Right()));
}

CSpeed COperator::Noisy(CSpeed target)
{
	CSpeed noisy = target;
	noisy.SetComponents(
		target.Left() + m_pUtil->GetUniformRandom(),
		target.Right() + m_pUtil->GetUniformRandom()
	);
	return noisy;
}

