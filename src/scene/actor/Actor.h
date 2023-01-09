#pragma once

#include "../Entity.h"
#include "base/ObjectLink.h"

#include "types/Matrix44.h"

namespace scene {

class Scene;

namespace actor {

MAJOR_CLASS( Actor, Entity )

	typedef float coord_t;
	typedef types::Vec2<coord_t> vec2_t;
	typedef types::Vec3 vec3_t;

	enum type_t {
		TYPE_MESH,
		TYPE_TEXT,
	};

	Actor( const type_t type, const std::string &name );
	~Actor();
	const type_t GetType() const {
		return m_type;
	}

	base::ObjectLink *m_renderer_object = NULL;

	/*
	types::Matrix44 & GetScaleMatrix();
	types::Matrix44 & GetTranslateMatrix();
	types::Matrix44 & GetRotateMatrix();

	void UpdateScale();
	void UpdatePosition();
	void UpdateRotation();
*/
	types::Matrix44 & GetWorldMatrix();

	void UpdateWorldMatrix();
	void UpdateMatrix();

	void SetScene( Scene *scene );
	Scene *GetScene();
protected:
	const type_t m_type;

	struct {
		types::Matrix44 world;
	} m_actor_matrices;
/*
	vec3_t m_position;
	vec3_t m_angle;
	vec3_t m_scale;

	struct {
		types::Matrix44 scale;
		types::Matrix44 translate;
		types::Matrix44 rotate;
	} m_matrices;
*/
	bool m_need_world_matrix_update = true;

	Scene *m_scene = NULL;
};

} /* namespace actor */
} /* namespace scene */
