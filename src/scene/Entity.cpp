#include "Entity.h"

namespace scene {

Entity::Entity() {
	m_matrices.scale.TransformScale( 1.0f, -1.0f, 1.0f ); // TODO: remove y inversion

	UpdatePosition();
	UpdateRotation();
	UpdateMatrix();
}

#define _XYZ_SETTER( _name, _var, _updates ) \
const types::Vec3& Entity::Get##_name() const {\
    return m_##_var;\
}\
const float Entity::Get##_name##X() const {\
    return m_##_var.x;\
}\
const float Entity::Get##_name##Y() const {\
    return m_##_var.y;\
}\
const float Entity::Get##_name##Z() const {\
    return m_##_var.z;\
}\
void Entity::Set##_name( const types::Vec3 & value ) {\
    if ( m_##_var != value ) {\
        m_##_var = value;\
        _updates;\
    }\
}\
void Entity::Set##_name##X( const float value ) {\
    if ( m_##_var.x != value ) {\
        m_##_var.x = value;\
        _updates;\
    }\
}\
void Entity::Set##_name##Y( const float value ) {\
    if ( m_##_var.y != value ) {\
        m_##_var.y = value;\
        _updates;\
    }\
}\
void Entity::Set##_name##Z( const float value ) {\
    if ( m_##_var.z != value ) {\
        m_##_var.z = value;\
        _updates;\
    }\
}

_XYZ_SETTER( Angle, angle, UpdateRotation(); )

_XYZ_SETTER( Position, position, UpdatePosition(); )

#undef _XYZ_SETTER

void Entity::UpdateRotation() {
	m_matrices.rotate.TransformRotate( m_angle.y, m_angle.x, m_angle.z );
	UpdateMatrix();
}

void Entity::UpdatePosition() {
	m_matrices.translate.TransformTranslate( m_position.x, m_position.y, m_position.z );
	UpdateMatrix();
}

void Entity::UpdateMatrix() {
	m_matrices.matrix = m_matrices.translate * m_matrices.rotate * m_matrices.scale;
}

types::Matrix44& Entity::GetMatrix() {
	return m_matrices.matrix;
}

const types::Buffer Entity::Serialize() const {
	types::Buffer buf;

	buf.WriteVec3( m_position );
	buf.WriteVec3( m_angle );

	return buf;
}

void Entity::Unserialize( types::Buffer buf ) {

	m_position = buf.ReadVec3();
	m_angle = buf.ReadVec3();

	UpdatePosition();
	UpdateRotation();
}

void Entity::Show() {
	if ( !m_is_visible ) {
		m_is_visible = true;
	}
}

void Entity::Hide() {
	if ( m_is_visible ) {
		m_is_visible = false;
	}
}

const bool Entity::IsVisible() const {
	return m_is_visible;
}

} /* namespace scene */
