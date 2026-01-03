#pragma once

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
	const types::Vec3& GetScale() const;
	void SetScene( Scene* scene );

	const float GetFov() const;
	void SetFov( const float fov );
	const float GetZNear() const;
	const float GetZFar() const;

	void UpdateUpTarget();
	void UpdateRotation() override;
	void UpdateProjection();
	void UpdateMatrix() override;

	void SetCustomAspectRatio( const float aspect_ratio );

protected:
	types::Vec3 m_scale = {
		0.0f,
		0.0f,
		0.0f
	};
	types::Vec3 m_up = {
		0.0f,
		0.0f,
		0.0f
	};
	types::Vec3 m_target = {
		0.0f,
		0.0f,
		0.0f
	};

	float m_fov = 90.0f;
	float m_raw_fov = 90.0f * M_PI / 180;
	float m_z_near = 0.1f;
	float m_z_far = 1.0f;

	const camera_type_t m_camera_type;

	types::Vec3 m_inverse_vector;
	struct {
		types::Matrix44 projection;
	} m_camera_matrices = {};

	Scene* m_scene = nullptr;

	bool m_is_custom_aspect_ratio = false;
	float m_custom_aspect_ratio = 0.0f;
	const float GetAspectRatio();

};

}
