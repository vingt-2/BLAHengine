#pragma once

/*Author: Vingt-2
	BLAengine
*/
#include "std.h"
class Vector3f
{
public:

	//Fields:
	float x,y,z;
	float sqrMagnitude;

	Vector3f(void);
	Vector3f(int x, int y, int z);
	~Vector3f(void);

	//Operator overloading and Operations:
	void operator+= (const Vector3f &secondVector);
	void operator-= (const Vector3f &secondVector);
	void operator*= (int scalar);
	const Vector3f operator+ (const Vector3f &secondVector);
	const Vector3f operator- (const Vector3f &secondVector);
	const Vector3f operator* (int scalar);
	const Vector3f operator/ (int scalar);

	float DotProduct(const Vector3f &secondVector);

	float GetMagnitude() const;
	Vector3f Normalized();

	// Non const function, modifies the object, dangerous but useful, beware !
	void Normalize();

private:
	void UpdateMagnitude();
};

