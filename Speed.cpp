#include <math.h>

#include "Common.h"
#include "Speed.h"

#include "KheperaInterface.h" // for Exception. TODO: move into separate file.

#define MAX_SPEED 30

CSpeed::CSpeed()
{
	SetVelocity(0);
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
	if (isnan(v)) throw Exception("Attempted to set NaN velocity!", -1);
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
	if (isnan(a)) throw Exception("Attempted to set NaN angle!", -1);

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
	double val = m_Velocity * (cos(m_Angle) - sin(m_Angle));
	if (isnan(val))
	{
		printf("Speed side val is NaN. V = %f, A = %f\n", m_Velocity, m_Angle);
	}
	return val;
}

double CSpeed::Right() const
{
	double val = m_Velocity * (cos(m_Angle) + sin(m_Angle));
	if (isnan(val))
	{
		printf("Speed side val is NaN. V = %f, A = %f\n", m_Velocity, m_Angle);
	}
	return val;
}

void CSpeed::SetComponents(double left, double right)
{
	if (left == 0 && right == 0)
	{
		SetAngle(0);
		SetVelocity(0);
		return;
	}

	double straight = (left + right) / 2; // cos(a)*v
	double turn = right - straight; // sin(a)*v

	double a = atan2(turn, straight);
	if (isnan(a)) throw Exception("Resulting angle is NaN. Left: " + std::to_string(left) + ", Right: " + std::to_string(left), 87);

	double v = straight / cos(a);
	if (v == 0) v = turn / sin(a);
	if (isnan(v)) throw Exception("Resulting velocity is NaN. Left: " + std::to_string(left) + ", Right: " + std::to_string(left), 87);

	SetAngle(a);
	SetVelocity(v);
}

void CSpeed::Limit()
{
	m_Velocity = fmin(m_Velocity, MAX_SPEED);
	m_Velocity = fmax(m_Velocity, -MAX_SPEED);
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
