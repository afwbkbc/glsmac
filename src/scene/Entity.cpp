#include "Entity.h"

namespace scene {

Entity::Entity() {
	m_matrices.scale.TransformScale( 1.0f, -1.0f, 1.0f );

	UpdatePosition();
	UpdateRotation();
	UpdateMatrix();
}

#define _XYZ_SETTER( _name, _var, _updates ) \
const types::Vec3 Entity::Get##_name() const {\
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
	m_##_var = value;\
	_updates;\
}\
void Entity::Set##_name##X( const float value ) {\
	m_##_var.x = value;\
	_updates;\
}\
void Entity::Set##_name##Y( const float value ) {\
	m_##_var.y = value;\
	_updates;\
}\
void Entity::Set##_name##Z( const float value ) {\
	m_##_var.z = value;\
	_updates;\
}

_XYZ_SETTER( Angle, angle, UpdateRotation(); UpdateMatrix() );
_XYZ_SETTER( Position, position, UpdatePosition(); UpdateMatrix() );

#undef _XYZ_SETTER

void Entity::UpdateRotation() {
	m_matrices.rotate.TransformRotate( m_angle.y, m_angle.x, m_angle.z );
}

void Entity::UpdatePosition() {
	m_matrices.translate.TransformTranslate( m_position.x , m_position.y, m_position.z );
}

void Entity::UpdateMatrix() {
	m_matrices.matrix = m_matrices.translate * m_matrices.rotate * m_matrices.scale;
}

types::Matrix44 & Entity::GetMatrix() {
	return m_matrices.matrix;
}



} /* namespace scene */
