/*Author: Vingt-2
	BLAengine
*/
#include "Vector3f.h"

Vector3f::Vector3f(void):
	x(0.0f),y(0.0f),z(0.0f),
	sqrMagnitude(0)
{}

Vector3f::Vector3f(int x, int y, int z):
	x(x),y(y),z(z),
	sqrMagnitude(x*x+y*y+z*z)
{}

Vector3f::~Vector3f(void) 
{}

//Operator overloads and Vector Operations:
void Vector3f::operator+= (const Vector3f &secondVector)
{
	this -> x = this-> x + secondVector.x ;
	this -> y = this-> y + secondVector.y ;
	this -> z = this-> z + secondVector.z ;
	UpdateMagnitude();
}
void Vector3f::operator-= (const Vector3f &secondVector)
{
	this -> x = this-> x - secondVector.x ;
	this -> y = this-> y - secondVector.y ;
	this -> z = this-> z - secondVector.z ;
	UpdateMagnitude();
}
void Vector3f::operator*= (int scalar)
{
	this -> x = this-> x * scalar ;
	this -> y = this-> y * scalar ;
	this -> z = this-> z * scalar ;
	UpdateMagnitude();
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

float Vector3f::DotProduct(const Vector3f &secondVector)
{
	return this->x*secondVector.x + this->y*secondVector.y + this->z*secondVector.z;
}

float Vector3f::GetMagnitude() const
{
	return sqrt(sqrMagnitude);
}

void Vector3f::Normalize()
{
	float magnitude = sqrt(sqrMagnitude);
	this->x *= 1 / magnitude;
	this->y *= 1 / magnitude;
	this->z *= 1 / magnitude;
	UpdateMagnitude();
}

Vector3f Vector3f::Normalized()
{
	Vector3f result(*this);
	result / sqrt(sqrMagnitude);
	return result;
}

void Vector3f::UpdateMagnitude()
{
	sqrMagnitude = x*x+y*y+z*z;
}

