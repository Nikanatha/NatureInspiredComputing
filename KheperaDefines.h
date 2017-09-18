#ifndef __KHEPERADEFINES_H
#define __KHEPERADEFINES_H

#define SIM_ONLY

// Define operating system
#define WINDOWS

#ifndef WINDOWS
	#define LINUX
#endif // !WINDOWS

// Khepera Interface Types
typedef enum ledstate
{
	Off = 0,
	On,
	Toggle
} LEDState;

typedef struct { int data[2]; } Int2;
typedef struct { int data[6]; } Int6;
typedef struct { int data[8]; } Int8;
typedef struct { int data[16]; } Int16;
typedef struct { int data[32]; } Int32;
typedef struct { int data[64]; } Int64;


#endif