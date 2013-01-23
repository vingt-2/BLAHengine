#pragma once
class Vector3f
{
public:

	//Fields:
	float x,y,z;

	Vector3f(void);
	Vector3f::Vector3f(int x, int y, int z);
	~Vector3f(void);

	//Operator overloading:
	void operator+= (const Vector3f &secondVector);
	void operator-= (const Vector3f &secondVector);
	void operator*= (int scalar);

	const Vector3f operator+ (const Vector3f &secondVector);
	const Vector3f operator- (const Vector3f &secondVector);
	const Vector3f operator* (int scalar);
	const Vector3f operator/ (int );

	//Class Functions:
	float GetMagnitude() const;
	float GetSqrMagnitude() const;
	Vector3f Normalized();

	// Non const function, BEWARE !
	void Normalize();
};

