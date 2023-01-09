#include <cmath>

#include "Vec4.h"
#include "Vec3.h"

namespace types {

Vec4::Vec4() {
	this->Set(0.0f,0.0f,0.0f,0.0f);
};

Vec4::Vec4(float x,float y,float z,float w) {
	this->Set(x,y,z,w);
};

void Vec4::Set(float x,float y,float z,float w) {
	this->x=x;
	this->y=y;
	this->z=z;
	this->w=w;
};

void Vec4::Normalize() {
	float Length = sqrt(this->x * this->x + this->y * this->y + this->z * this->z + this->w * this->w);
	this->x /= Length;
	this->y /= Length;
	this->z /= Length;
	this->w /= Length;
};

void Vec4::Conjugate() {
	this->x=-this->x;
	this->y=-this->y;
	this->z=-this->z;
	//this->w=this->w;
};

Vec4 Vec4::operator * (const Vec4 operand) {
	Vec4 r;

	r.w = (this->w * operand.w) - (this->x * operand.x) - (this->y * operand.y) - (this->z * operand.z);
	r.x = (this->x * operand.w) + (this->w * operand.x) + (this->y * operand.z) - (this->z * operand.y);
	r.y = (this->y * operand.w) + (this->w * operand.y) + (this->z * operand.x) - (this->x * operand.z);
	r.z = (this->z * operand.w) + (this->w * operand.z) + (this->x * operand.y) - (this->y * operand.x);

	return r;
};

Vec4 Vec4::operator * (const Vec3 operand) {
	Vec4 r;

	r.w = - (this->x * operand.x) - (this->y * operand.y) - (this->z * operand.z);
	r.x =   (this->w * operand.x) + (this->y * operand.z) - (this->z * operand.y);
	r.y =   (this->w * operand.y) + (this->z * operand.x) - (this->x * operand.z);
	r.z =   (this->w * operand.z) + (this->x * operand.y) - (this->y * operand.x);

	return r;
};

} /* namespace types */

