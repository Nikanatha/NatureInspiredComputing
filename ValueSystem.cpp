#include "ValueSystem.h"

#define DISTANCE_LIMIT (0.2 * CLOSE_SENSOR_VAL)

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
	// evaluate speeds
	double straightSpeed = (calculated.speed.left + calculated.speed.right) / 2.0;
	double rightTurnSpeed = (calculated.speed.left - calculated.speed.right);

	// evaluate sensor data
	double leftSum = calculated.sensors.data[0] + 2*calculated.sensors.data[1] + 3*calculated.sensors.data[2];
	double rightSum = calculated.sensors.data[3] + 2*calculated.sensors.data[4] + 3*calculated.sensors.data[5];
	double frontSum = calculated.sensors.data[2] + calculated.sensors.data[3];

	if (frontSum > DISTANCE_LIMIT) // impending frontal collision
	{
		straightSpeed = fmin(straightSpeed, 0);
	}
	else	// free road
	{
		straightSpeed = abs(straightSpeed);

		// gradually speed up
		if (straightSpeed < MAX_SPEED) straightSpeed *= 1.5;

	}
	if(frontSum < DISTANCE_LIMIT) // freeeeeee
	{
		straightSpeed = fmax(straightSpeed, MAX_SPEED/2);
	}

	if (leftSum < rightSum) // obstacle is to the left, turn right
	{
		rightTurnSpeed = abs(rightTurnSpeed);
		rightTurnSpeed = fmax(rightTurnSpeed, 2 * (double)MAX_SPEED*(leftSum / (6 * SENSOR_VAL_RANGE)));
	}
	else // obstacle is to the right, turn left
	{
		rightTurnSpeed = -abs(rightTurnSpeed);
		rightTurnSpeed = fmin(rightTurnSpeed, -2 * (double)MAX_SPEED*(rightSum / (6 * SENSOR_VAL_RANGE)));
	}


	// assemble resulting speed
	SIOSet correction;

	correction.sensors = calculated.sensors;
	correction.speed.left = straightSpeed - rightTurnSpeed / 2;
	correction.speed.right = straightSpeed + rightTurnSpeed / 2;

	return correction;
}
