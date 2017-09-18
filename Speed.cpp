#include <math.h>

#include "Common.h"
#include "Speed.h"

#include "KheperaInterface.h" // for Exception. TODO: move into separate file.

double CSpeed::MaxSpeed = 20;

CSpeed::CSpeed()
{
	SetComponents(0, 0);
}

CSpeed::CSpeed(double left, double right)
{
	SetComponents(left, right);
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


double CSpeed::Left() const
{
	return m_Left*m_Velocity;
}

double CSpeed::Right() const
{
	return m_Right*m_Velocity;
}

void CSpeed::SetComponents(double left, double right)
{
	if (left == 0 && right == 0)
	{
		m_Left = 0;
		m_Right = 0;
		m_Velocity = 0;
		return;
	}

	double straight = (left + right) / 2; // cos(a)*v
	double turn = right - straight; // sin(a)*v

	double a = atan2(turn, straight);
	
	double v = straight / cos(a);
	if (v == 0) v = turn / sin(a);

	m_Velocity = v;
	m_Left = left / v;
	m_Right = right / v;
}

void CSpeed::Limit()
{
	double factor = m_Velocity / MaxSpeed;
	if (factor > 1)
	{
		m_Velocity /= abs(factor);
	}
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
	prod = *this;
	prod.m_Velocity *= factor;
	return prod;
}

CSpeed CSpeed::operator/(double factor)
{
	CSpeed div;
	div = *this;
	div.m_Velocity /= factor;
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
	this->m_Velocity *= factor;
	return *this;
}

CSpeed & CSpeed::operator/=(double factor)
{
	this->m_Velocity /= factor;
	return *this;
}

bool CSpeed::operator<(const CSpeed & other) const
{
	return abs(this->Velocity()) < (other.Velocity());
}
