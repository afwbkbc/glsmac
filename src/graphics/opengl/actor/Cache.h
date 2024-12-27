#pragma once

#include <GL/glew.h>
#include <unordered_set>
#include <map>

#include "Actor.h"

#include "types/mesh/Types.h"

namespace types::texture {
class Texture;
}

namespace types::mesh {
class Rectangle;
}

namespace graphics {
namespace opengl {

class FBO;

CLASS( Cache, Actor )

	Cache( OpenGL* opengl, scene::actor::Actor* actor );
	~Cache();

	void AddCacheChild( Actor* cache_child );
	void RemoveCacheChild( Actor* cache_child );
	void SetCacheChildZIndex( Actor* cache_child, const float zindex );

	void UpdateCache();

	void OnWindowResize() override;

	void SetEffectiveArea( const types::Vec2< types::mesh::coord_t >& top_left, const types::Vec2< types::mesh::coord_t >& bottom_right, const types::mesh::coord_t z );

protected:

	void DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) override;

private:
	bool m_is_update_needed = true;

	std::unordered_set< Actor* > m_cache_children = {};
	std::map< float, std::vector< Actor* > > m_cache_children_by_zindex;

	void AddCacheChildToZIndexSet( Actor* gl_actor, const float zindex );
	void RemoveCacheChildFromZIndexSet( Actor* gl_actor, const float zindex );

	types::Vec2< uint16_t > m_fbo_size = {
		0,
		0
	};
	FBO* m_fbo;

	types::texture::Texture* m_texture = nullptr;
	types::mesh::Rectangle* m_mesh = nullptr;

private:
	friend class Actor;
	void UpdateCacheImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr );

};

}
}
