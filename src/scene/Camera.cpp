#include "Camera.h"

#include "engine/Engine.h"

#define ANGLE_CENTER ( - M_PI / 2 )

namespace scene {

Camera::Camera( const camera_type_t camera_type )
	: Entity()
	, m_camera_type( camera_type ) {
	//m_inverse_vector.Set( -1.0f, -1.0f, 1.0f );
	m_inverse_vector.Set( 1.0f, 1.0f, 1.0f );
	//m_matrices.scale.TransformScale( 1.0, -1.0, -1.0 );

	m_angle.Set( ANGLE_CENTER, ANGLE_CENTER, ANGLE_CENTER );

	SetFov( m_fov );
	SetScale(
		{
			1.0f,
			1.0f,
			1.0f
		}
	);

	UpdateProjection();
	UpdateRotation();
	UpdateMatrix();
}

void Camera::SetScale( types::Vec3 scale ) {
	if ( scale != m_scale ) {
		//m_matrices.scale.TransformScale( -scale.x, -scale.y, -scale.z );
		m_matrices.scale.TransformScale( -scale.x, scale.y, scale.z );
		UpdateMatrix();
		m_scale = scale;
	}
}

const types::Vec3& Camera::GetScale() const {
	return m_scale;
}

void Camera::SetScene( Scene* scene ) {
	ASSERT( m_scene == NULL || scene == NULL, "duplicate scene set" );
	m_scene = scene;
}

const float Camera::GetAspectRatio() {
	if ( m_is_custom_aspect_ratio ) {
		return m_custom_aspect_ratio;
	}
	else {
		return g_engine->GetGraphics()->GetAspectRatio();
	}
}

void Camera::UpdateProjection() {
	switch ( m_camera_type ) {
		case CT_ORTHOGRAPHIC: {
			m_camera_matrices.projection.ProjectionOrtho2D( GetAspectRatio(), m_z_near, m_z_far );
			break;
		}
		case CT_PERSPECTIVE: {
			m_camera_matrices.projection.ProjectionPerspective( GetAspectRatio(), m_raw_fov, m_z_near, m_z_far );
			break;
		}
		default: {
			THROW( "invalid camera type" );
		}
	}
}

void Camera::UpdateRotation() {
	UpdateUpTarget();
	m_matrices.rotate.TransformCameraRotate( m_target * m_inverse_vector, m_up * m_inverse_vector );
}

void Camera::UpdateMatrix() {
	Entity::UpdateMatrix();
	m_matrices.matrix = m_camera_matrices.projection * m_matrices.matrix;
	if ( m_scene ) {
		for ( auto it = m_scene->m_actors.begin() ; it < m_scene->m_actors.end() ; ++it ) {
			( *it )->UpdateWorldMatrix();
		}
	}
}

void Camera::SetCustomAspectRatio( const float aspect_ratio ) {
	ASSERT( !m_is_custom_aspect_ratio, "custom aspect ratio already set, is it a bug/typo?" );
	m_is_custom_aspect_ratio = true;
	m_custom_aspect_ratio = aspect_ratio;
	UpdateProjection();
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
	types::Vec3 axis( 0.0f, 1.0f, 0.0f );
	types::Vec3 view( 1.0f, 0.0f, 0.0f );
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
