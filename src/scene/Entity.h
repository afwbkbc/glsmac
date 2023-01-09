#pragma once

#include "base/Base.h"

#include "types/Matrix44.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#endif

namespace scene {

class Entity : public base::Base {
public:
	Entity();

	// GetAngle(), SetAngle(), etc
#define _XYZ_SETTER( _name ) \
	const types::Vec3 Get##_name() const;\
	const float Get##_name##X() const;\
	const float Get##_name##Y() const;\
	const float Get##_name##Z() const;\
	void Set##_name( const types::Vec3 & value );\
	void Set##_name##X( const float value );\
	void Set##_name##Y( const float value );\
	void Set##_name##Z( const float value );

	_XYZ_SETTER( Position );
	_XYZ_SETTER( Angle );

#undef _XYZ_SETTER

	types::Matrix44 & GetMatrix();

	virtual void UpdatePosition();
	virtual void UpdateRotation();
	virtual void UpdateMatrix();
protected:
	types::Vec3 m_position;
	types::Vec3 m_angle;

	struct {
		types::Matrix44 rotate;
		types::Matrix44 translate;
		types::Matrix44 scale;
		types::Matrix44 matrix;
	} m_matrices;

};

} /* namespace scene */
