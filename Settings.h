#ifndef __Settings_H__
#define __Settings_H__

class CRbfSettings
{
public:
	CRbfSettings();

public:
	double Sigma();
	double Learn();
	void SetLearn(double val);
	//double Decay();
	//void SetDecay();
	//double MinActivation();
	//void SetMinActivation();
	int MaxNodes();
	void SetMaxNodes(int val);

private:
	int m_MaxNodes;
};

class CMscSettings
{
public:
	CMscSettings();

public:
	double MaxSpeed;
};

#endif