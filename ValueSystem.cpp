#include "ValueSystem.h"

CValueSystem::CValueSystem(CKheperaUtility * pUtil) : CThreadableBase(pUtil)
{

}

void CValueSystem::DoCycle()
{
	// evaluate current results
	SIOSet controllerResults = m_pUtil->GetLastNetworkResult();
	SIOSet correction = Correct(controllerResults);
	m_pUtil->SetCorrectedResult(correction);
}

SIOSet CValueSystem::Correct(SIOSet calculated)
{
	// TODO: replace current dummy function.
	return calculated;
}
