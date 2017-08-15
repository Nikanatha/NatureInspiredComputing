#ifndef __Speed_H__
#define __Speed_H__

class CSpeed
{
public:
	CSpeed();
	CSpeed(double velocity, double angle);

	double Velocity() const;
	void SetVelocity(double v);
	void IncreaseVelocity(double v);

	double Angle() const;
	void SetAngle(double a);
	void IncreaseAngle(double a);

	double Left() const;
	double Right() const;
	void SetComponents(double left, double right);

	void Limit();

	CSpeed operator+(CSpeed other);
	CSpeed operator-(CSpeed other);
	CSpeed operator*(double factor);
	CSpeed operator/(double factor);

	CSpeed& operator+=(CSpeed other);
	CSpeed& operator-=(CSpeed other);
	CSpeed& operator*=(double factor);
	CSpeed& operator/=(double factor);

	bool operator<(const CSpeed& other) const;

private:
	double m_Velocity;
	double m_Angle;
};

#endif