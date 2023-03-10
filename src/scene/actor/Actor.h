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
		TYPE_SPRITE,
		TYPE_INSTANCED_SPRITE,
		TYPE_MESH,
		TYPE_INSTANCED_MESH,
		TYPE_TEXT,
		TYPE_IMAGE,
		TYPE_SOUND,
	};

	Actor( const type_t type, const std::string &name );
	virtual ~Actor();
	const type_t GetType() const {
		return m_type;
	}

	base::ObjectLink *m_graphics_object = NULL;

	virtual types::Matrix44 & GetWorldMatrix();

	virtual void UpdateWorldMatrix();
	virtual void UpdateMatrix();

	void SetScene( Scene *scene );
	Scene *GetScene();
	
	typedef std::pair< types::Vec3, types::Vec3 > coordinate_limits_t;
	void SetCoordinateLimits( const coordinate_limits_t limits );
	const coordinate_limits_t& GetCoordinateLimits() const;
	
	typedef uint32_t render_flag_t;
	static constexpr render_flag_t RF_NONE = 0;
	static constexpr render_flag_t RF_USE_TINT = 1 << 0;
	static constexpr render_flag_t RF_IGNORE_LIGHTING = 1 << 1;
	static constexpr render_flag_t RF_IGNORE_CAMERA = 1 << 2;
	static constexpr render_flag_t RF_IGNORE_DEPTH = 1 << 3;
	static constexpr render_flag_t RF_USE_COORDINATE_LIMITS = 1 << 4;
	
	void SetRenderFlags( const render_flag_t render_flags );
	const render_flag_t GetRenderFlags() const;
	
	virtual const types::Buffer Serialize() const;
	virtual void Unserialize( types::Buffer buf );
	
protected:
	const type_t m_type;

	struct {
		types::Matrix44 world;
	} m_actor_matrices;
	
	bool m_need_world_matrix_update = true;

	Scene *m_scene = NULL;

	render_flag_t m_render_flags = RF_NONE;
	coordinate_limits_t m_coordinate_limits = {};
	
};

} /* namespace actor */
} /* namespace scene */
