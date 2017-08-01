#ifndef __Speed_H__
#define __Speed_H__

class CSpeed
{
public:
	CSpeed();
	CSpeed(double velocity, double angle);

	double Velocity();
	void SetVelocity(double v);
	void IncreaseVelocity(double v);

	double Angle();
	void SetAngle(double a);
	void IncreaseAngle(double a);

	double Left();
	double Right();
	void SetComponents(double left, double right);

	CSpeed operator+(CSpeed other);
	CSpeed operator-(CSpeed other);
	CSpeed operator*(double factor);
	CSpeed operator/(double factor);

	CSpeed& operator+=(CSpeed other);
	CSpeed& operator-=(CSpeed other);
	CSpeed& operator*=(double factor);
	CSpeed& operator/=(double factor);

private:
	double m_Velocity;
	double m_Angle;
};

#endif