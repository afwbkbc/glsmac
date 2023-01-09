#pragma once

#include <cstddef>

#include "Entity.h"

#include "types/Matrix44.h"

namespace scene {

class Scene;

MAJOR_CLASS( Camera, Entity )
	Camera();

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
	types::Vec3 m_up;
	types::Vec3 m_target;

	float m_fov;
	float m_raw_fov;
	float m_z_near = 0.01;
	float m_z_far = 200.0;

	types::Vec3 m_inverse_vector;
	struct {
		types::Matrix44 projection;
	} m_camera_matrices;

	Scene *m_scene = NULL;
};

} /* namespace scene */

#include "Scene.h"
