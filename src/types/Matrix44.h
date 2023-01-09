#pragma once

#include "Vec3.h"

namespace types {

class Matrix44 {
public:
	Matrix44();
	Matrix44(const Matrix44& source);
	Matrix44(float m00,float m01,float m02,float m03,float m10,float m11,float m12,float m13,float m20,float m21,float m22,float m23,float m30,float m31,float m32,float m33);
	float m[4][4];
	void CopyFrom(const Matrix44& source);
	void Identity();
	void TransformScale(const float x,const float y,const float z);
	void TransformTranslate(const float x,const float y,const float z);
	void TransformRotate(const float x,const float y,const float z);
	void TransformCameraRotate(const Vec3 target,const Vec3 up);
	void ProjectionPerspective(const float aspect_ratio,const float fov,const float znear,const float zfar);
	void ProjectionOrtho2D(const float znear,const float zfar);
	Matrix44 operator * (const Matrix44 operand);
	void operator *= (const Matrix44 operand);
};

} /* namespace types */
