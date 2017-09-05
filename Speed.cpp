#include <math.h>

#include "Common.h"
#include "Speed.h"

#include "KheperaInterface.h" // for Exception. TODO: move into separate file.

#define MIN_SPEED 5
#define MAX_SPEED 30

CSpeed::CSpeed()
{
	m_VelocityFlipped = false;
	SetAngularComponents(0, 0);
}

CSpeed::CSpeed(double velocity, double angle)
{
	m_VelocityFlipped;
	SetAngularComponents(velocity, angle);
}

double CSpeed::Velocity() const
{
	return m_VelocityFlipped ? -m_Velocity : m_Velocity;
}

void CSpeed::SetVelocity(double v)
{
	if (isnan(v)) throw Exception("Attempted to set NaN velocity!", -1);
	m_Velocity = m_VelocityFlipped ? -v : v;
	Limit();
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

	if (abs(simple) > PI / 2)
	{
		m_VelocityFlipped = !m_VelocityFlipped;
		simple += simple < 0 ? PI : -PI;
	}

	m_Angle = simple;
	Limit();
}

void CSpeed::IncreaseAngle(double a)
{
	SetAngle(m_Angle + a);
}

double CSpeed::Left() const
{
	double val = Velocity() * (cos(Angle()) - sin(Angle()));
	if (isnan(val))
	{
		printf("Speed side val is NaN. V = %f, A = %f\n", m_Velocity, m_Angle);
	}
	return val;
}

double CSpeed::Right() const
{
	double val = Velocity() * (cos(Angle()) + sin(Angle()));
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
		SetAngularComponents(0, 0);
		return;
	}

	double straight = (left + right) / 2; // cos(a)*v
	double turn = right - straight; // sin(a)*v

	double a = atan2(turn, straight);
	if (isnan(a)) throw Exception("Resulting angle is NaN. Left: " + std::to_string(left) + ", Right: " + std::to_string(left), 87);

	double v = straight / cos(a);
	if (v == 0) v = turn / sin(a);
	if (isnan(v)) throw Exception("Resulting velocity is NaN. Left: " + std::to_string(left) + ", Right: " + std::to_string(left), 87);

	SetAngularComponents(v, a);
}

void CSpeed::SetAngularComponents(double v, double a)
{
	SetVelocity(v);
	SetAngle(a);
}

void CSpeed::Limit()
{
	if (m_VelocityFlipped)
	{
		m_Velocity = -m_Velocity;
		m_VelocityFlipped = false;
	}

	if (m_Velocity < 0)
	{
//		m_Velocity = fmin(m_Velocity, -MIN_SPEED);
		m_Velocity = fmax(m_Velocity, -MAX_SPEED);
		return;
	}

	m_Velocity = fmin(m_Velocity, MAX_SPEED);
//	m_Velocity = fmax(m_Velocity, MIN_SPEED);
}

CSpeed CSpeed::operator+(CSpeed other)
{
	CSpeed sum;
	double left = this->Left() + other.Left();
	double right = this->Right() + other.Right();
	sum.SetComponents(left, right);
	return sum;
}

CSpeed CSpeed::operator-(CSpeed other)
{
	CSpeed diff;
	double left = this->Left() - other.Left();
	double right = this->Right() - other.Right();
	diff.SetComponents(left, right);
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
