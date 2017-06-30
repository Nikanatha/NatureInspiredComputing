#include <math.h>
#include <chrono>
#include "Operator.h"

COperator::COperator(CKheperaUtility * pUtil)
{
}

COperator::~COperator()
{
	Stop();
}

void COperator::Start()
{
	m_bStopFlag = false;
	m_pThread = new std::thread(&COperator::Run, this);
	m_pThread->detach();
}

void COperator::Stop()
{
	m_bStopFlag = true;
}

void COperator::Run()
{
	while (!m_bStopFlag)
	{
		// convert target speed to noisy int for khepera
		SSpeed target = m_pUtil->GetLastNetworkResult().speed;
		Int2 rounded = NoiseRound(target);
		m_pUtil->SetSpeed(rounded);
		std::this_thread::sleep_for(std::chrono::milliseconds(50)); //TOD: adjust timing
	}
}

Int2 COperator::NoiseRound(SSpeed target)
{
	Int2 rounded;
	rounded.data[0] = target.left + m_pUtil->GetUniformRandom(0.5, -0.5);
	rounded.data[1] = target.right + m_pUtil->GetUniformRandom(0.5, -0.5);
	return rounded;
}

