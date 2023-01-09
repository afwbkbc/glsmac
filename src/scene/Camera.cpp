#include "Camera.h"

#include "engine/Engine.h"

#define ANGLE_CENTER ( - M_PI / 2 )

namespace scene {

Camera::Camera() : Entity() {
	m_inverse_vector.Set( -1.0f, -1.0f, 1.0f );
	m_matrices.scale.TransformScale( 1.0, -1.0, 1.0 );

	m_angle.Set( ANGLE_CENTER, ANGLE_CENTER, ANGLE_CENTER );

	SetFov( 90.0 );

	UpdateProjection();
	UpdateRotation();
	UpdateMatrix();
}

void Camera::SetScene( Scene *scene ) {
	if ( m_scene != NULL && scene != NULL )
		throw CameraError( "duplicate scene set" );
	m_scene = scene;
}

void Camera::UpdateProjection() {
	m_camera_matrices.projection.ProjectionPerspective( g_engine->GetRenderer()->GetAspectRatio(), m_raw_fov, m_z_near, m_z_far );
}

void Camera::UpdateRotation() {
	UpdateUpTarget();
	m_matrices.rotate.TransformCameraRotate( m_target * m_inverse_vector, m_up * m_inverse_vector );
}

void Camera::UpdateMatrix() {
	Entity::UpdateMatrix();
	m_matrices.matrix = m_camera_matrices.projection * m_matrices.matrix;
	if ( m_scene )
		for ( auto it = m_scene->m_actors.begin() ; it < m_scene->m_actors.end() ; ++it )
			(*it)->UpdateWorldMatrix();
}

const float Camera::GetFov() const {
	return m_fov;
}

void Camera::SetFov( const float fov ) {
	m_fov = fov;
	m_raw_fov = m_fov * M_PI / 180;
	UpdateProjection();
	UpdateMatrix();
}

const float Camera::GetZNear() const {
	return m_z_near;
}

const float Camera::GetZFar() const {
	return m_z_far;
}

void Camera::UpdateUpTarget() {
	types::Vec3 axis( 0.0f, 1.0f, 0.0f);
	types::Vec3 view( 1.0f, 0.0f, 0.0f);
	view.Rotate( m_angle.x, axis );
	view.Normalize();
	axis = axis ^ view;
	axis.Normalize();
	view.Rotate( m_angle.y, axis );
	view.Normalize();
	m_up = view;
	m_target = view ^ axis;
}

} /* namespace scene */
