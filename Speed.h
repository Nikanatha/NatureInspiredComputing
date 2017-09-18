#ifndef __Speed_H__
#define __Speed_H__

class CSpeed
{
public:
	static double MaxSpeed;

public:
	CSpeed();
	CSpeed(double left, double right);

	double Velocity() const;
	void SetVelocity(double v);
	void IncreaseVelocity(double v);

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
	double m_Left;
	double m_Right;
	double m_Velocity;
};

#endif