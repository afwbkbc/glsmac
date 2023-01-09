#include <cmath>
#include <cstdint>

#include "Matrix44.h"

namespace types {

Matrix44::Matrix44() {
/*	for (uint8_t y=0;y<4;y++)
		for (uint8_t x=0;x<4;x++)
			this->m[x][y]=0.0f;*/
	this->Identity();
};

Matrix44::Matrix44(const Matrix44& source) {
	this->CopyFrom(source);
};

Matrix44::Matrix44(float m00,float m01,float m02,float m03,float m10,float m11,float m12,float m13,float m20,float m21,float m22,float m23,float m30,float m31,float m32,float m33) {
	this->m[0][0]=m00; this->m[0][1]=m01; this->m[0][2]=m02; this->m[0][3]=m03;
	this->m[1][0]=m10; this->m[1][1]=m11; this->m[1][2]=m12; this->m[1][3]=m13;
	this->m[2][0]=m20; this->m[2][1]=m21; this->m[2][2]=m22; this->m[2][3]=m23;
	this->m[3][0]=m30; this->m[3][1]=m31; this->m[3][2]=m32; this->m[3][3]=m33;
};

void Matrix44::Identity() {
	for (uint8_t y=0;y<4;y++)
		for (uint8_t x=0;x<4;x++)
			this->m[x][y]=1.0f;
};

void Matrix44::CopyFrom(const Matrix44& source) {
	for (uint8_t y=0;y<4;y++)
		for (uint8_t x=0;x<4;x++)
			this->m[x][y]=source.m[x][y];
};

void Matrix44::TransformScale(const float x,const float y,const float z) {
	this->m[0][0] = x;		this->m[0][1] = 0.0f;	this->m[0][2] = 0.0f;	this->m[0][3] = 0.0f;
	this->m[1][0] = 0.0f;	this->m[1][1] = y;		this->m[1][2] = 0.0f;	this->m[1][3] = 0.0f;
	this->m[2][0] = 0.0f;	this->m[2][1] = 0.0f;	this->m[2][2] = z;		this->m[2][3] = 0.0f;
	this->m[3][0] = 0.0f;	this->m[3][1] = 0.0f;	this->m[3][2] = 0.0f;	this->m[3][3] = 1.0f;
};

void Matrix44::TransformTranslate(const float x,const float y,const float z) {
	this->m[0][0] = 1.0f;	this->m[0][1] = 0.0f;	this->m[0][2] = 0.0f;	this->m[0][3] = x;
	this->m[1][0] = 0.0f;	this->m[1][1] = 1.0f;	this->m[1][2] = 0.0f;	this->m[1][3] = y;
	this->m[2][0] = 0.0f;	this->m[2][1] = 0.0f;	this->m[2][2] = 1.0f;	this->m[2][3] = z;
	this->m[3][0] = 0.0f;	this->m[3][1] = 0.0f;	this->m[3][2] = 0.0f;	this->m[3][3] = 1.0f;
};

void Matrix44::TransformRotate(const float x,const float y,const float z) {
	Matrix44 rx,ry,rz;

	rx.m[0][0] = 1.0f;	rx.m[0][1] = 0.0f;	rx.m[0][2] = 0.0f;	rx.m[0][3] = 0.0f;
	rx.m[1][0] = 0.0f;	rx.m[1][1] = cosf(x);	rx.m[1][2] = -sinf(x);	rx.m[1][3] = 0.0f;
	rx.m[2][0] = 0.0f;	rx.m[2][1] = sinf(x);	rx.m[2][2] = cosf(x);	rx.m[2][3] = 0.0f;
	rx.m[3][0] = 0.0f;	rx.m[3][1] = 0.0f;	rx.m[3][2] = 0.0f;	rx.m[3][3] = 1.0f;

	ry.m[0][0] = cosf(y);	ry.m[0][1] = 0.0f;	ry.m[0][2] = -sinf(y);	ry.m[0][3] = 0.0f;
	ry.m[1][0] = 0.0f;	ry.m[1][1] = 1.0f;	ry.m[1][2] = 0.0f;	ry.m[1][3] = 0.0f;
	ry.m[2][0] = sinf(y);	ry.m[2][1] = 0.0f;	ry.m[2][2] = cosf(y);	ry.m[2][3] = 0.0f;
	ry.m[3][0] = 0.0f;	ry.m[3][1] = 0.0f;	ry.m[3][2] = 0.0f;	ry.m[3][3] = 1.0f;

	rz.m[0][0] = cosf(z);	rz.m[0][1] = -sinf(z);	rz.m[0][2] = 0.0f;	rz.m[0][3] = 0.0f;
	rz.m[1][0] = sinf(z);	rz.m[1][1] = cosf(z);	rz.m[1][2] = 0.0f;	rz.m[1][3] = 0.0f;
	rz.m[2][0] = 0.0f;	rz.m[2][1] = 0.0f;	rz.m[2][2] = 1.0f;	rz.m[2][3] = 0.0f;
	rz.m[3][0] = 0.0f;	rz.m[3][1] = 0.0f;	rz.m[3][2] = 0.0f;	rz.m[3][3] = 1.0f;

	*this=rx*ry*rz;
};

void Matrix44::TransformCameraRotate(const Vec3 target,const Vec3 up) {
	Vec3 N=target;
	N.Normalize();
	Vec3 U=up;
	U.Normalize();
	U=U^target;
	Vec3 V=N^U;

	this->m[0][0] = U.x;	this->m[0][1] = U.y;	this->m[0][2] = U.z;	this->m[0][3] = 0.0f;
	this->m[1][0] = V.x;	this->m[1][1] = V.y;	this->m[1][2] = V.z;	this->m[1][3] = 0.0f;
	this->m[2][0] = N.x;	this->m[2][1] = N.y;	this->m[2][2] = N.z;	this->m[2][3] = 0.0f;
	this->m[3][0] = 0.0f;	this->m[3][1] = 0.0f;	this->m[3][2] = 0.0f;	this->m[3][3] = 1.0f;
};

void Matrix44::ProjectionPerspective(const float aspect_ratio,const float fov,const float znear,const float zfar) {
    const float zRange     = znear - zfar;
    const float tanHalfFOV = tanf(fov / 2.0f);

	this->m[0][0] = 1.0f/(tanHalfFOV * aspect_ratio);	this->m[0][1] = 0.0f;	this->m[0][2] = 0.0f;	this->m[0][3] = 0.0;
	this->m[1][0] = 0.0f;	this->m[1][1] = 1.0f/tanHalfFOV;		this->m[1][2] = 0.0f;	this->m[1][3] = 0.0;
	this->m[2][0] = 0.0f;	this->m[2][1] = 0.0f;	this->m[2][2] = (-znear -zfar)/zRange;	this->m[2][3] = 2.0f * zfar*znear/zRange;
	this->m[3][0] = 0.0f;	this->m[3][1] = 0.0f;			this->m[3][2] = 1.0f;			this->m[3][3] = 0.0;
};

void Matrix44::ProjectionOrtho2D(const float znear,const float zfar) {
	this->m[0][0]=2.0f;	this->m[0][1]=0.0f;	this->m[0][2]=0.0f;	this->m[0][3]=-1.0f;
	this->m[1][0]=0.0f;	this->m[1][1]=-2.0f;this->m[1][2]=0.0f;	this->m[1][3]=1.0f;
	this->m[2][0]=0.0f;	this->m[2][1]=0.0f;	this->m[2][2]=2.0f/(zfar-znear);	this->m[2][2]=(znear+zfar)/(znear-zfar);
	this->m[3][0]=0.0f;	this->m[3][1]=0.0f;	this->m[3][2]=0.0f;	this->m[3][3]=1.0f;
};

Matrix44 Matrix44::operator * (const Matrix44 operand) {
	Matrix44 ret;
	for (uint8_t i = 0 ; i < 4 ; i++) {
		for (uint8_t j = 0 ; j < 4 ; j++) {
			ret.m[i][j] =	this->m[i][0] * operand.m[0][j] +
							this->m[i][1] * operand.m[1][j] +
							this->m[i][2] * operand.m[2][j] +
							this->m[i][3] * operand.m[3][j];
		}
	}
	return ret;
};

void Matrix44::operator *= (const Matrix44 operand) {
	for (uint8_t i = 0 ; i < 4 ; i++) {
		for (uint8_t j = 0 ; j < 4 ; j++) {
			this->m[i][j] =	this->m[i][0] * operand.m[0][j] +
							this->m[i][1] * operand.m[1][j] +
							this->m[i][2] * operand.m[2][j] +
							this->m[i][3] * operand.m[3][j];
		}
	}
};

} /* namespace types */
