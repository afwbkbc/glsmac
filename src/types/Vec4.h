#pragma once

#include "Vec3.h"

namespace types {

class Vec4 {
public:
	Vec4();
	Vec4(float x,float y,float z,float w);
	void Set(float x,float y,float z,float w);
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
	void Normalize();
	void Conjugate();
	Vec4 operator * (const Vec4 operand);
	Vec4 operator * (const Vec3 operand);
};

} /* namespace types */
