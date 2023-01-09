#pragma once

#include <cfloat>
#include <cmath>

#include "Vec2.h"

namespace types {

#define DATATYPE float

class Vec3 {
public:
	DATATYPE x = 0.0f;
	DATATYPE y = 0.0f;
	DATATYPE z = 0.0f;

	Vec3() {

	};


	Vec3(DATATYPE x,DATATYPE y,DATATYPE z) {
		this->Set(x,y,z);
	};


	~Vec3() {

	};


	Vec3(const Vec2<float> source) {
		this->Set(source.x,source.y,0.0);
	}

	void Set(DATATYPE x,DATATYPE y,DATATYPE z) {
		this->x=x;
		this->y=y;
		this->z=z;
	};


	Vec3 operator=(const Vec2<float> source) {
		return Vec3( source.x, source.y, 0.0 );
	};


	bool operator==(const Vec3 rhs) {
	    return(this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
	};


	Vec3 operator/(const Vec3 rhs) {
	    return Vec3( this->x / rhs.x,
	                 this->y / rhs.y,
	                 this->z / rhs.z);
	};


	Vec3 operator*(const Vec3 rhs) {
	    return Vec3( this->x * rhs.x,
	                 this->y * rhs.y,
	                 this->z * rhs.z);
	};


	Vec3 operator+(const Vec3 rhs) {
	    return Vec3( this->x + rhs.x,
	                 this->y + rhs.y,
	                 this->z + rhs.z);
	};


	Vec3 operator-(const Vec3 rhs) {
	    return Vec3( this->x - rhs.x,
	                 this->y - rhs.y,
	                 this->z - rhs.z);
	};


	void operator+=(const Vec3 rhs) {
		this->x+=rhs.x;
		this->y+=rhs.y;
		this->z+=rhs.z;
	};


	void operator-=(const Vec3 rhs) {
		this->x-=rhs.x;
		this->y-=rhs.y;
		this->z-=rhs.z;
	};


	void operator*=(const Vec3 rhs) {
		this->x*=rhs.x;
		this->y*=rhs.y;
		this->z*=rhs.z;
	};


	void operator/=(const Vec3 rhs) {
		this->x/=rhs.x;
		this->y/=rhs.y;
		this->z/=rhs.z;
	};


	Vec3 operator/(const DATATYPE scalar) {
	    return Vec3( this->x / scalar,
	                 this->y / scalar,
	                 this->z / scalar);
	};


	Vec3 operator*(const DATATYPE scalar) {
	    return Vec3( this->x * scalar,
	                 this->y * scalar,
	                 this->z * scalar);
	};


	Vec3 operator+(const DATATYPE scalar) {
	    return Vec3( this->x + scalar,
	                 this->y + scalar,
	                 this->z + scalar);
	};


	Vec3 operator-(const DATATYPE scalar) {
	    return Vec3( this->x - scalar,
	                 this->y - scalar,
	                 this->z - scalar);
	};


	void operator+=(const DATATYPE scalar) {
		this->x+=scalar;
		this->y+=scalar;
		this->z+=scalar;
	};


	void operator-=(const DATATYPE scalar) {
		this->x-=scalar;
		this->y-=scalar;
		this->z-=scalar;
	};


	void operator*=(const DATATYPE scalar) {
		this->x*=scalar;
		this->y*=scalar;
		this->z*=scalar;
	};


	void operator/=(const DATATYPE scalar) {
		this->x/=scalar;
		this->y/=scalar;
		this->z/=scalar;
	};


	Vec3 operator^(const Vec3 rhs) {
	    return Vec3( this->y * rhs.z - this->z * rhs.y,
	                 this->z * rhs.x - this->x * rhs.z,
	                 this->x * rhs.y - this->y * rhs.x);
	};


	DATATYPE operator|(const Vec3 rhs) {
	    return (this->x * rhs.x +
	            this->y * rhs.y +
	            this->z * rhs.z);
	};


	DATATYPE Length() {
	    return DATATYPE(sqrt( x*x + y*y + z*z ));
	};


	void NormalizeBy(const DATATYPE by) {
		const DATATYPE Length = this->Length()/by;
		if (Length>FLT_EPSILON) {
			this->x /= Length;
			this->y /= Length;
			this->z /= Length;
		}
	};


	void Normalize() {
		this->NormalizeBy(1.0f);
	};

	void Rotate(const DATATYPE angle,const Vec3 axe);

};

#undef DATATYPE

} /* namespace types */
