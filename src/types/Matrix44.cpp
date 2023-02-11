#include <cmath>
#include <cstdint>
#include <iostream>

#include "Matrix44.h"

namespace types {

Matrix44::Matrix44() {
/*	for (uint8_t y=0;y<4;y++)
		for (uint8_t x=0;x<4;x++)
			m[x][y]=0.0f;*/
	Identity();
};

Matrix44::Matrix44(const Matrix44& source) {
	CopyFrom(source);
};

Matrix44::Matrix44(float m00,float m01,float m02,float m03,float m10,float m11,float m12,float m13,float m20,float m21,float m22,float m23,float m30,float m31,float m32,float m33) {
	m[0][0]=m00; m[0][1]=m01; m[0][2]=m02; m[0][3]=m03;
	m[1][0]=m10; m[1][1]=m11; m[1][2]=m12; m[1][3]=m13;
	m[2][0]=m20; m[2][1]=m21; m[2][2]=m22; m[2][3]=m23;
	m[3][0]=m30; m[3][1]=m31; m[3][2]=m32; m[3][3]=m33;
};

void Matrix44::Identity() {
	for (uint8_t y=0;y<4;y++)
		for (uint8_t x=0;x<4;x++)
			m[x][y]=1.0f;
};

void Matrix44::CopyFrom(const Matrix44& source) {
	for (uint8_t y=0;y<4;y++)
		for (uint8_t x=0;x<4;x++)
			m[x][y]=source.m[x][y];
};

void Matrix44::TransformScale(const float x,const float y,const float z) {
	m[0][0] = x;		m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0f;
	m[1][0] = 0.0f;	m[1][1] = y;		m[1][2] = 0.0f;	m[1][3] = 0.0f;
	m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = z;		m[2][3] = 0.0f;
	m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
};

void Matrix44::TransformTranslate(const float x,const float y,const float z) {
	m[0][0] = 1.0f;	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = x;
	m[1][0] = 0.0f;	m[1][1] = 1.0f;	m[1][2] = 0.0f;	m[1][3] = y;
	m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = 1.0f;	m[2][3] = z;
	m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
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

	m[0][0] = U.x;	m[0][1] = U.y;	m[0][2] = U.z;	m[0][3] = 0.0f;
	m[1][0] = V.x;	m[1][1] = V.y;	m[1][2] = V.z;	m[1][3] = 0.0f;
	m[2][0] = N.x;	m[2][1] = N.y;	m[2][2] = N.z;	m[2][3] = 0.0f;
	m[3][0] = 0.0f;	m[3][1] = 0.0f;	m[3][2] = 0.0f;	m[3][3] = 1.0f;
};

void Matrix44::ProjectionPerspective(const float aspect_ratio,const float fov,const float znear,const float zfar) {
    const float zRange     = znear - zfar;
    const float tanHalfFOV = tanf(fov / 2.0f);

	m[0][0] = 1.0f/(tanHalfFOV * aspect_ratio);	m[0][1] = 0.0f;	m[0][2] = 0.0f;	m[0][3] = 0.0;
	m[1][0] = 0.0f;	m[1][1] = 1.0f/tanHalfFOV;		m[1][2] = 0.0f;	m[1][3] = 0.0;
	m[2][0] = 0.0f;	m[2][1] = 0.0f;	m[2][2] = (-znear -zfar)/zRange;	m[2][3] = 2.0f * zfar*znear/zRange;
	m[3][0] = 0.0f;	m[3][1] = 0.0f;			m[3][2] = 1.0f;			m[3][3] = 0.0;
};

void Matrix44::ProjectionOrtho2D(const float aspect_ratio,const float znear,const float zfar) {
	m[0][0]=2.0f/aspect_ratio;	m[0][1]=0.0f;	m[0][2]=0.0f;	m[0][3]=-1.0f;
	m[1][0]=0.0f;	m[1][1]=-2.0f;m[1][2]=0.0f;	m[1][3]=1.0f;
	m[2][0]=0.0f;	m[2][1]=0.0f;	m[2][2]=2.0f/(zfar-znear);	m[2][2]=(znear+zfar)/(znear-zfar);
	m[3][0]=0.0f;	m[3][1]=0.0f;	m[3][2]=0.0f;	m[3][3]=1.0f;
};

Matrix44 Matrix44::operator * (const Matrix44 operand) {
	Matrix44 ret;
	for (uint8_t i = 0 ; i < 4 ; i++) {
		for (uint8_t j = 0 ; j < 4 ; j++) {
			ret.m[i][j] =	m[i][0] * operand.m[0][j] +
							m[i][1] * operand.m[1][j] +
							m[i][2] * operand.m[2][j] +
							m[i][3] * operand.m[3][j];
		}
	}
	return ret;
};

void Matrix44::operator *= (const Matrix44 operand) {
	for (uint8_t i = 0 ; i < 4 ; i++) {
		for (uint8_t j = 0 ; j < 4 ; j++) {
			m[i][j] =	m[i][0] * operand.m[0][j] +
							m[i][1] * operand.m[1][j] +
							m[i][2] * operand.m[2][j] +
							m[i][3] * operand.m[3][j];
		}
	}
};

} /* namespace types */
