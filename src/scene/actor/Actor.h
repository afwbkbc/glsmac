#pragma once

#include "../Entity.h"
#include "base/ObjectLink.h"

#include "types/Matrix44.h"

namespace scene {

class Scene;

namespace actor {

CLASS( Actor, Entity )

	typedef float coord_t;
	typedef types::Vec2<coord_t> vec2_t;
	typedef types::Vec3 vec3_t;

	enum type_t {
		TYPE_MESH,
		TYPE_INSTANCED_MESH,
		TYPE_TEXT,
		TYPE_IMAGE,
		TYPE_SOUND,
	};

	Actor( const type_t type, const std::string &name );
	~Actor();
	const type_t GetType() const {
		return m_type;
	}

	base::ObjectLink *m_graphics_object = NULL;

	virtual types::Matrix44 & GetWorldMatrix();

	virtual void UpdateWorldMatrix();
	virtual void UpdateMatrix();

	void SetScene( Scene *scene );
	Scene *GetScene();
protected:
	const type_t m_type;

	struct {
		types::Matrix44 world;
	} m_actor_matrices;
	
	bool m_need_world_matrix_update = true;

	Scene *m_scene = NULL;
};

} /* namespace actor */
} /* namespace scene */
