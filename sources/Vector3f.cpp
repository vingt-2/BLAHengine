#include "Vector3f.h"

Vector3f::Vector3f(void):
	x(0.0f),y(0.0f),z(0.0f)
{}

Vector3f::Vector3f(int x, int y, int z):
	x(x),y(y),z(z)
{
}

Vector3f::~Vector3f(void) 
{}

//Operator overloaders:
void Vector3f::operator+= (const Vector3f &secondVector)
{
	this -> x = this-> x + secondVector.x ;
	this -> y = this-> y + secondVector.y ;
	this -> z = this-> z + secondVector.z ;
}
void Vector3f::operator-= (const Vector3f &secondVector)
{
	this -> x = this-> x - secondVector.x ;
	this -> y = this-> y - secondVector.y ;
	this -> z = this-> z - secondVector.z ;
}
void Vector3f::operator*= (int scalar)
{
	this -> x = this-> x * scalar ;
	this -> y = this-> y * scalar ;
	this -> z = this-> z * scalar ;
}
const Vector3f Vector3f::operator+ (const Vector3f &secondVector)
{
	Vector3f addResult(*this);
	addResult += secondVector;
	return addResult;
}
const Vector3f Vector3f::operator- (const Vector3f &secondVector)
{
	Vector3f substractResult(*this);
	substractResult -= secondVector;
	return substractResult;
}
const Vector3f Vector3f::operator*(int scalar)
{
	Vector3f multResult(*this);
	multResult *= scalar;
	return multResult;
}
const Vector3f Vector3f::operator/(int scalar)
{
	Vector3f divResult(*this);
	divResult *= scalar;
	return divResult;
}


float Vector3f::GetMagnitude() const
{
	float sqrMagnitude = GetSqrMagnitude();
	return sqrMagnitude * sqrMagnitude;
}

float Vector3f::GetSqrMagnitude() const
{
	return x*x + y*y + z*z ;
	Vector3f bitte();
}


void Vector3f::Normalize()
{
	float magnitude = GetMagnitude();
	this->x *= 1 / magnitude;
	this->y *= 1 / magnitude;
	this->z *= 1 / magnitude;
}

Vector3f Vector3f::Normalized()
{
	Vector3f result(*this);
	result / GetMagnitude();
	return result;
}

