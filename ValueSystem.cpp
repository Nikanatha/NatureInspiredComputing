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
	SIOSet correction = calculated;

	// evaluate sensor data in movement direction
	EProximity proximity;
	proximity = calculated.sensors[AngleToDirection(calculated.speed.Angle())];

    if(calculated.speed.Velocity() == 0) correction.speed.IncreaseVelocity(m_pUtil->GetUniformRandom(0.5, 1));

    if(calculated.sensors.Collision() && calculated.sensors[Direction_Back] != Proximity_Collision)
    {
        correction.speed.SetVelocity(-1);
    }

	if (proximity > Proximity_Near)
	{	// don't go there, you'll collide!
		// go anywhere else, just not there. Preferably forward-ish.
		correction.speed.IncreaseAngle(pow(-1, round(m_pUtil->GetUniformRandom(0, 1)))*(PI / 6 + m_pUtil->GetUniformRandom(0, PI / 6)));
	}
    else if (calculated.speed.Velocity() < m_pUtil->MaxSpeed)
	{	// you're fine, go faster
		correction.speed *= 1.2;
	}

    if(abs(correction.speed.Angle())) correction.speed.SetAngle(m_pUtil->GetUniformRandom(-PI/2, PI/2));
	if (correction.speed.Velocity() > m_pUtil->MaxSpeed) correction.speed.SetVelocity(m_pUtil->MaxSpeed);
    
	return correction;
}
