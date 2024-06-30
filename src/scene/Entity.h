#pragma once

#include "types/Serializable.h"

#include "types/Matrix44.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /* pi */
#endif

namespace scene {

CLASS( Entity, types::Serializable )

	Entity();

	// GetAngle(), SetAngle(), etc
#define _XYZ_SETTER( _name ) \
    virtual const types::Vec3& Get##_name() const;\
    virtual const float Get##_name##X() const;\
    virtual const float Get##_name##Y() const;\
    virtual const float Get##_name##Z() const;\
    virtual void Set##_name( const types::Vec3 & value );\
    virtual void Set##_name##X( const float value );\
    virtual void Set##_name##Y( const float value );\
    virtual void Set##_name##Z( const float value );

	_XYZ_SETTER( Position );
	_XYZ_SETTER( Angle );

#undef _XYZ_SETTER

	types::Matrix44& GetMatrix();

	virtual void UpdatePosition();
	virtual void UpdateRotation();
	virtual void UpdateMatrix();

	struct matrices_t {
		types::Matrix44 rotate;
		types::Matrix44 translate;
		types::Matrix44 scale;
		types::Matrix44 matrix;
	};

	virtual const types::Buffer Serialize() const override;
	virtual void Unserialize( types::Buffer buf ) override;

	void Show();
	void Hide();
	const bool IsVisible() const;

protected:
	types::Vec3 m_position = {
		0.0,
		0.0,
		0.0
	};
	types::Vec3 m_angle = {
		0.0,
		0.0,
		0.0
	};

	matrices_t m_matrices = {};

	bool m_is_visible = true;

};

}
