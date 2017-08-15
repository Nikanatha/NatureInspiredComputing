#include <math.h>

#include "Common.h"
#include "Speed.h"

#define MAX_SPEED 5

CSpeed::CSpeed()
{
	SetVelocity(10);
	SetAngle(0);
}

CSpeed::CSpeed(double velocity, double angle)
{
	SetVelocity(velocity);
	SetAngle(angle);
}

double CSpeed::Velocity() const
{
	return m_Velocity;
}

void CSpeed::SetVelocity(double v)
{
	m_Velocity = v;
}

void CSpeed::IncreaseVelocity(double v)
{
	SetVelocity(m_Velocity + v);
}

double CSpeed::Angle() const
{
	return m_Angle;
}

void CSpeed::SetAngle(double a)
{
	double simple = a;
	while (simple > PI) simple -= 2 * PI;
	while (simple < -PI) simple += 2 * PI;
	m_Angle = simple;
}

void CSpeed::IncreaseAngle(double a)
{
	SetAngle(m_Angle + a);
}

double CSpeed::Left() const
{
	return m_Velocity * (cos(m_Angle) - sin(m_Angle));
}

double CSpeed::Right() const
{
	return m_Velocity * (cos(m_Angle) + sin(m_Angle));
}

void CSpeed::SetComponents(double left, double right)
{
	double straight = (left + right) / 2; // cos(a)*v
	double turn = right - straight; // sin(a)*v

	double a = atan2(turn, straight);
	double v = straight / cos(a);
	if (v == 0) v = turn / sin(a);

	SetAngle(a);
	SetVelocity(v);
}

void CSpeed::Limit()
{
	m_Velocity = fmin(m_Velocity, MAX_SPEED);
}

CSpeed CSpeed::operator+(CSpeed other)
{
	CSpeed sum;
	sum.SetComponents(this->Left() + other.Left(), this->Right() + other.Right());
	return sum;
}

CSpeed CSpeed::operator-(CSpeed other)
{
	CSpeed diff;
	diff.SetComponents(this->Left() - other.Left(), this->Right() - other.Right());
	return diff;
}

CSpeed CSpeed::operator*(double factor)
{
	CSpeed prod;
	prod.SetAngle(this->Angle());
	prod.SetVelocity(this->Velocity()*factor);
	return prod;
}

CSpeed CSpeed::operator/(double factor)
{
	CSpeed div;
	div.SetAngle(this->Angle());
	div.SetVelocity(this->Velocity()/factor);
	return div;
}

CSpeed & CSpeed::operator+=(CSpeed other)
{
	SetComponents(this->Left() + other.Left(), this->Right() + other.Right());
	return *this;
}

CSpeed & CSpeed::operator-=(CSpeed other)
{
	SetComponents(this->Left() - other.Left(), this->Right() - other.Right());
	return *this;
}

CSpeed & CSpeed::operator*=(double factor)
{
	SetVelocity(this->Velocity()*factor);
	return *this;
}

CSpeed & CSpeed::operator/=(double factor)
{
	SetVelocity(this->Velocity()/factor);
	return *this;
}

bool CSpeed::operator<(const CSpeed & other) const
{
	return this->Velocity() < other.Velocity();
}
