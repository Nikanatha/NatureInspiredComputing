#ifndef __Settings_H__
#define __Settings_H__

class CRbfSettings
{
public:
	CRbfSettings();

public:
	double Sigma;
	double Learn;
	double Decay;
	double MinActivation;
	int MaxNodes;
};

class CMscSettings
{
public:
	CMscSettings();

public:
	double MaxSpeed;
};

#endif