#include "KheperaInterface.h"

// For serial communication
#ifdef LINUX
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif // LINUX

#ifdef WINDOWS

#endif // WINDOWS

#include <iostream>

#include <cstring> // for memset
#include <cstdlib> // for strtol

KheperaInterface::KheperaInterface(const string& device, bool simulate)
{
	this->simulate = simulate;
	
	if (!simulate)
		setupSerial(device.c_str()); 
	
	// Causes an exception to be thrown if robot is not connected
	getVersion();
	
	try { getGripperVersion(); hasgripper = true; }
	catch (std::runtime_error& e) { hasgripper = false; }

	try { getCameraVersion(); hascamera = true; }
	catch (std::runtime_error& e) { hascamera = false; }
}

KheperaInterface::~KheperaInterface()
{
	if (!simulate)
	{
		// Restore original serial configuration
#ifdef LINUX
		tcsetattr(fileno(f), TCSANOW, &oldtio);
#endif // LINUX
#ifdef WINDOWS
		// close file
		//khepStream.close();
		CloseHandle(serial);
#endif // WINDOWS

	}
}

void KheperaInterface::setupSerial(const char* dev)
{
#ifdef LINUX
	f = fopen(dev, "r+");
	if (!f)
		throw Exception("Could not open device");
		
	int fd = fileno(f);
	tcflush(fd, TCIOFLUSH);
		
	struct termios oldtio;
	if (tcgetattr(fd, &oldtio) != 0)
		throw Exception("Could not get serial configuration");
		   
	struct termios newtio = oldtio;
	newtio.c_iflag &= ~(IGNBRK | IGNPAR | BRKINT | INPCK | ISTRIP | ICRNL | INLCR | IXON | IXOFF | IMAXBEL); 
	newtio.c_oflag &= ~OPOST;
	newtio.c_cflag &= ~(CSIZE | PARENB|  CRTSCTS);
	newtio.c_cflag |=(CS8 | CSTOPB | CREAD);
	newtio.c_lflag &= ~(ISIG | ICANON | XCASE | ECHO | FLUSHO | PENDIN | IEXTEN);		     
	cfsetispeed(&newtio, 9600);
	cfsetospeed(&newtio, 9600);
	newtio.c_cc[VMIN] = 0;
	newtio.c_cc[VTIME] = 5;
	if (tcsetattr(fd,TCSANOW, &newtio) != 0)
		throw Exception("Could not set serial configuration");

#endif // LINUX
#ifdef WINDOWS
	serial = CreateFile(L"COM5", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (serial == INVALID_HANDLE_VALUE)
	{
		throw Exception("Could not open device");
	}

	// Do some basic settings
	DCB serialParams = { 0 };
	serialParams.DCBlength = sizeof(serialParams);

	GetCommState(serial, &serialParams);
	serialParams.BaudRate = 9600;
	serialParams.ByteSize = 8;
	serialParams.StopBits = 1;
	serialParams.Parity = 0;
	SetCommState(serial, &serialParams);

	// Set timeouts
	COMMTIMEOUTS timeout = { 0 };
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 50;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;

	SetCommTimeouts(serial, &timeout);

#endif // WINDOWS

}

bool KheperaInterface::hasGripper()
{ return hasgripper; }

bool KheperaInterface::hasCamera()
{ return hascamera; }

void KheperaInterface::sendCommand(const char* cmd, char* answer, int maxAnswerLength)
{
#ifdef LINUX
	if (fputs(cmd, f) < 0)
		std::cout << "Error " << errno() << std::endl;
		throw Exception("Serial writing failed");
	if (!fgets(answer, maxAnswerLength, f))
		throw Exception("Serial reading failed");
#endif // LINUX

#ifdef WINDOWS
	BOOL bSuccess;

	bSuccess = WriteFile(serial, cmd, strlen(cmd), 0, 0);
	if(!bSuccess)
		throw Exception("Serial writing failed");
	
	bSuccess = ReadFile(serial, answer, maxAnswerLength, 0, 0);
	if (!bSuccess)
		throw Exception("Serial reading failed");

#endif // WINDOWS

}		

void KheperaInterface::setSpeedControllerPID(int proportional, int integral, int differential)
{
	if (!simulate)
	{
		sprintf(buffer, "A,%d,%d,%d\n", proportional, integral, differential);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("a\r\n", buffer, strlen("a\r\n")) != 0)
			throw Exception("Setting speed controller PID failed");
	}
}

string KheperaInterface::getVersion()
{
	if (!simulate)
	{
		sendCommand("B\n", buffer, sizeof(buffer));
		if (buffer[0] != 'b' || buffer[1] != ',')
			throw Exception("Reading version failed");
		return string(&buffer[2]);
	}
	return "Simulation";
}

void KheperaInterface::setPosition(int leftPos, int rightPos)
{
	if (!simulate)
	{
		sprintf(buffer, "C,%d,%d\n", leftPos, rightPos);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("c\r\n", buffer, strlen("c\r\n")) != 0)
			throw Exception("Setting position failed");
	}
}

void KheperaInterface::setSpeed(int leftMotor, int rightMotor)
{
	if (!simulate)
	{
		sprintf(buffer, "D,%d,%d\n", leftMotor, rightMotor);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("d\r\n", buffer, strlen("d\r\n")) != 0)
			throw Exception("Setting speed failed");
	}
}

Int2 KheperaInterface::getSpeed()
{
	Int2 result = {{0,0}};
	if (!simulate)
	{
		sendCommand("E\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "e,%d,%d\r\n", &result.data[0], &result.data[1]) != 2)
			throw Exception("Reading speed failed");
	}
	return result;
}

void KheperaInterface::setPositionControllerPID(int proportional, int integral, int differential)
{
	if (!simulate)
	{
		sprintf(buffer, "F,%d,%d,%d\n", proportional, integral, differential);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("f\r\n", buffer, strlen("f\r\n")) != 0)
			throw Exception("Setting position controller PID failed");		
	}
}

void KheperaInterface::setPositionCounter(int leftCounter, int rightCounter)
{
	if (!simulate)
	{
		sprintf(buffer, "G,%d,%d\n", leftCounter, rightCounter);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("g\r\n", buffer, strlen("g\r\n")) != 0)
			throw Exception("Setting position counter failed");
	}
}

Int2 KheperaInterface::getPosition()
{
	Int2 result = {{0,0}};
	if (!simulate)
	{
		sendCommand("H\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "h,%d,%d\r\n", &result.data[0], &result.data[1]) != 2)
			throw Exception("Reading position failed");
	}
	return result;
}
int KheperaInterface::getADInput(int channel)
{
	int result = 0;
	if (!simulate)
	{
		sprintf(buffer, "I,%d\n", channel);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (sscanf(buffer, "i,%d\r\n", &result) != 1)
			throw Exception("Reading AD input failed");
	}
	return result;
}

void KheperaInterface::setSpeedControllerProfile(int maxLeftSpeed, int leftAccel, int maxRightSpeed, int rightAccel)
{
	if (!simulate)
	{
		sprintf(buffer, "J,%d,%d,%d,%d\n", maxLeftSpeed, leftAccel, maxRightSpeed, rightAccel);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("j\r\n", buffer, strlen("j\r\n")) != 0)
			throw Exception("Setting speed controller profile failed");
	}
}

Int6 KheperaInterface::getMotionControllerStatus()
{
	Int6 result = {{0,0,0,0,0,0}};
	if (!simulate)
	{
		sendCommand("K\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "k,%d,%d,%d,%d,%d,%d\r\n", 
			&result.data[0], &result.data[1], &result.data[2], 
			&result.data[3], &result.data[4], &result.data[5]) != 6)
		throw Exception("Reading motion controller status failed");
	}
	return result;
}

void KheperaInterface::setLEDState(int number, LEDState state)
{
	if (!simulate)
	{
		sprintf(buffer, "L,%d,%d\n", number, state);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("l\r\n", buffer, strlen("l\r\n")) != 0)
			throw Exception("Setting led failed");
	}
}

Int8 KheperaInterface::getProximitySensors()
{
	Int8 result = {{0,0,0,0,0,0,0,0}};
	if (!simulate)
	{
		sendCommand("N\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "n,%d,%d,%d,%d,%d,%d,%d,%d\r\n", 
				&result.data[0], &result.data[1], &result.data[2], &result.data[3],
				&result.data[4], &result.data[5], &result.data[6], &result.data[7]) != 8)
			throw Exception("Reading proximity sensors failed");
	}	
	return result;	
}

Int8 KheperaInterface::getAmbientSensors()
{
	Int8 result = {{0,0,0,0,0,0,0,0}};
	if (!simulate)
	{
		sendCommand("O\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "o,%d,%d,%d,%d,%d,%d,%d,%d\r\n", 
				&result.data[0], &result.data[1], &result.data[2], &result.data[3],
				&result.data[4], &result.data[5], &result.data[6], &result.data[7]) != 8)
			throw Exception("Reading light sensors failed");
	}	
	return result;	
}

void KheperaInterface::setMotorPWM(int leftPWM, int rightPWM)
{
	if (!simulate)
	{
		sprintf(buffer, "P,%d,%d\n", leftPWM, rightPWM);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("p\r\n", buffer, strlen("p\r\n")) != 0)
			throw Exception("Setting motor pwm failed");
	}
}

int KheperaInterface::readExtensionBusByte(int address)
{
	int result = 0;
	if (!simulate)
	{
		sprintf(buffer, "R,%d\n", address);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (sscanf(buffer, "r,%d\r\n", &result) != 1)
			throw Exception("Reading extension bus byte failed");
	}
	return result;
}

void KheperaInterface::writeExtensionBusByte(int address, int value)
{
	if (!simulate)
	{
		sprintf(buffer, "W,%d,%d\n", address, value);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("w\r\n", buffer, strlen("w\r\n")) != 0)
			throw Exception("Writing extension bus byte failed");
	}
}


string KheperaInterface::getGripperVersion()
{
	if (!simulate) 
	{
		sendCommand("T,1,B\n", buffer, sizeof(buffer));
		if (strncmp("t,1,b,", buffer, strlen("t,1,b,")) != 0)
			throw Exception("Reading gripper version failed");
		return string(&buffer[6]);
	}
	return "Simulation";
}

void KheperaInterface::setGripperState(bool opened)
{
	if (!simulate)
	{
		sprintf(buffer, "T,1,D,%d\n", (opened ? 0 : 1));
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("t,1,d\r\n", buffer, strlen("t,1,d\r\n")) != 0)
			throw Exception("Setting gripper state failed");
	}
}

void KheperaInterface::setGripperPosition(int position)
{
	if (!simulate)
	{
		sprintf(buffer, "T,1,E,%d\n", position);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("t,1,e\r\n", buffer, strlen("t,1,e\r\n")) != 0)
			throw Exception("Setting gripper position failed");
	}
}

int KheperaInterface::getGripperImpedance()
{
	int result = 0;
	if (!simulate)
	{
		sendCommand("T,1,F\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,1,f,%d\r\n", &result) != 1)
			throw Exception("Reading gripper impedance failed");
	}
	return result;
}

int KheperaInterface::getGripperPhotoSensor()
{
	int result = 0;
	if (!simulate)
	{
		sendCommand("T,1,G\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,1,g,%d\r\n", &result) != 1)
			throw Exception("Reading gripper photosensor failed");
	}
	return result;
}

bool KheperaInterface::getGripperState()
{
	bool result = false;
	if (!simulate)
	{
		int tmp;
		sendCommand("T,1,H,0\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,1,h,%d\r\n", &tmp) != 1)
			throw Exception("Reading gripper state failed");
		result = (tmp != 0);
	}
	return result;
}

int KheperaInterface::getGripperPosition()
{
	int result = 0;
	if (!simulate)
	{
		sendCommand("T,1,H,1\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,1,h,%d\r\n", &result) != 1)
			throw Exception("Reading gripper position failed");
	}
	return result;	
}

int KheperaInterface::getGripperJumpers()
{
	int result = 0;
	if (!simulate)
	{
		sendCommand("T,1,J\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,1,j,%d\r\n", &result) != 1)
			throw Exception("Reading gripper jumpers failed");
	}
	return result;
}

string KheperaInterface::getCameraVersion()
{
	if (!simulate) 
	{
		sendCommand("T,2,B\n", buffer, sizeof(buffer));
		if (strncmp("t,2,b,", buffer, strlen("t,2,b,")) != 0)
			throw Exception("Reading camera version failed");
		return string(&buffer[6]);
	}
	return "Simulation";
		
}

int KheperaInterface::getCameraLightIntensity()
{
	Int2 tmpResult = {{0,0}};
	if (!simulate)
	{
		sendCommand("T,2,L\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,2,l,%d,%d\r\n", &tmpResult.data[0], &tmpResult.data[1]) != 2)
			throw Exception("Reading camera light intensity failed");
	}
	return ((tmpResult.data[0]<<8)&0xFF00) | (tmpResult.data[1]&0xFF);
}

bool KheperaInterface::parseIntList(char* text, int expectedCount, int result[])
{
	char* tmp = text, *next;
	for (int i = 0; i < expectedCount; i++)
	{
		result[i] = strtol(tmp, &next, 0);
		if (tmp == next)
			return false;
		if (next[0] != ',' && next[0] != '\r')
			return false;
		if (next[0] == '\r' && i != expectedCount-1)
			return false;
		tmp = next+1;
	}
	if (next[0] != '\r' || next[1] != '\n')
		return false;
	return true;	
}

Int32 KheperaInterface::getCameraImageLowRes()
{
	Int32 result;
	memset(&result, 0, sizeof(result));
	if (!simulate)
	{
		sendCommand("T,2,M\n", buffer, sizeof(buffer));
		if (strncmp("t,2,m,", buffer, strlen("t,2,m,")) != 0)
			throw Exception("Reading camera image failed (low res)");
		if (!parseIntList(&buffer[6], 32, result.data))
			throw Exception("Reading camera image failed (low res)");
	}
	return result;
}

Int64 KheperaInterface::getCameraImage()
{
	Int64 result;
	memset(&result, 0, sizeof(result));
	if (!simulate)
	{
		sendCommand("T,2,N\n", buffer, sizeof(buffer));
		if (strncmp("t,2,n,", buffer, strlen("t,2,n,")) != 0)
			throw Exception("Reading camera image failed");
		if (!parseIntList(&buffer[6], 64, result.data))
			throw Exception("Reading camera image failed");
	}
	return result;
}

int KheperaInterface::getCameraMaxIntensityPixelIndex()
{
	int result = 0;
	if (!simulate)
	{
		sendCommand("T,2,O\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,2,o,%d\r\n", &result) != 1)
			throw Exception("Reading index of pixel with max intensity from camera failed");
	}
	return result;
}

int KheperaInterface::getCameraMinIntensityPixelIndex()
{
	int result = 0;
	if (!simulate)
	{
		sendCommand("T,2,P\n", buffer, sizeof(buffer));
		if (sscanf(buffer, "t,2,p,%d\r\n", &result) != 1)
			throw Exception("Reading index of pixel with min intensity from camera failed");
	}
	return result;
	
}

Int8 KheperaInterface::getCameraEightPixels(int startPixelIndex)
{
	Int8 result = {{0,0,0,0,0,0,0,0}};
	if (!simulate)
	{
		sprintf(buffer, "T,2,Q,%d\n", startPixelIndex);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (sscanf(buffer, "t,2,q,%d,%d,%d,%d,%d,%d,%d,%d\r\n", 
				&result.data[0], &result.data[1], &result.data[2], &result.data[3],
				&result.data[4], &result.data[5], &result.data[6], &result.data[7]) != 8)
			throw Exception("Reading eight pixels from camera failed");
	}	
	return result;
}

Int16 KheperaInterface::getCameraSixteenPixels(int startPixelIndex)
{
	Int16 result;
	memset(&result, 0, sizeof(result));
	if (!simulate)
	{
		sprintf(buffer, "T,2,R,%d\n", startPixelIndex);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("t,2,r,", buffer, strlen("t,2,r,")) != 0)
			throw Exception("Reading 16 pixels from camera failed");
		if (!parseIntList(&buffer[6], 16, result.data))
			throw Exception("Reading 16 pixels from camera failed");
	}
	return result;
}

Int32 KheperaInterface::getCameraTwoSubScannedImage()
{
	Int32 result;
	memset(&result, 0, sizeof(result));
	if (!simulate)
	{
		sendCommand("T,2,S\n", buffer, sizeof(buffer));
		if (strncmp("t,2,s,", buffer, strlen("t,2,s,")) != 0)
			throw Exception("Reading 32 pixels from camera failed");
		if (!parseIntList(&buffer[6], 32, result.data))
			throw Exception("Reading 32 pixels from camera failed");
	}
	return result;
}

Int16 KheperaInterface::getCameraFourSubScannedImage()
{
	Int16 result;
	memset(&result, 0, sizeof(result));
	if (!simulate)
	{
		sendCommand("T,2,T\n", buffer, sizeof(buffer));
		if (strncmp("t,2,t,", buffer, strlen("t,2,t,")) != 0)
			throw Exception("Reading subscanned image from camera failed");
		if (!parseIntList(&buffer[6], 16, result.data))
			throw Exception("Reading subscanned image from camera failed");
	}
	return result;	
}

void KheperaInterface::setCameraReadingPeriod(int period)
{
	if (!simulate)
	{
		sprintf(buffer, "T,2,U,%d\n", period);
		sendCommand(buffer, buffer, sizeof(buffer));
		if (strncmp("t,2,u", buffer, strlen("t,2,u")) != 0)
			throw Exception("Setting camera reading period failed");
	}
}

std::runtime_error Exception(std::string message, int error)
{
	message.append(" Error = ");
	message.append(std::to_string(error));
	message.append(". ");

	std::cout << message << std::endl;
	return std::runtime_error(message);
}
