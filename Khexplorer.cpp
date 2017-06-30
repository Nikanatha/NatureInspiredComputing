// Khexplorer.cpp : Defines the entry point for the console application.
//

#include "KheperaUtility.h"
#include "Controller.h"
#include "ValueSystem.h"
#include "Operator.h"

int main()
{
	CKheperaUtility util;
	CController control(&util);
	CValueSystem values(&util);
	COperator operate(&util);

	// stop is called in destructor
	control.Start();
	values.Start();
	operate.Start();

	// keep running
	bool bRunning = true;
	while(bRunning) 
	{
		// possibly look for stop command
	}

	// stop our robot!
	control.Stop();
	values.Stop();
	operate.Stop();

	Int2 stop;
	stop.data[0] = 0;
	stop.data[1] = 0;
	util.SetSpeed(stop);

    return 0;
}

