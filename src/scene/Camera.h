#pragma once

#include <cstddef>

#include "Entity.h"

#include "types/Matrix44.h"

namespace scene {

class Scene;

CLASS( Camera, Entity )
	enum camera_type_t {
		CT_ORTHOGRAPHIC,
		CT_PERSPECTIVE,
	};

	Camera( const camera_type_t camera_type );

	void SetScale( types::Vec3 scale );
	void SetScene( Scene *scene );

	const float GetFov() const;
	void SetFov( const float fov );
	const float GetZNear() const;
	const float GetZFar() const;

	void UpdateUpTarget();
	void UpdateRotation();
	void UpdateProjection();
	void UpdateMatrix();
protected:
	types::Vec3 m_up = { 0.0, 0.0, 0.0 };
	types::Vec3 m_target = { 0.0, 0.0, 0.0 };

	float m_fov = 90.0;
	float m_raw_fov = 90.0 * M_PI / 180;
	float m_z_near = 0.01;
	float m_z_far = 200.0;

	const camera_type_t m_camera_type;
	
	types::Vec3 m_inverse_vector;
	struct {
		types::Matrix44 projection;
	} m_camera_matrices = {};

	Scene *m_scene = nullptr;
};

} /* namespace scene */

#include "Scene.h"
