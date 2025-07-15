#pragma once

#include <GL/glew.h>
#include <unordered_set>
#include <map>
#include <vector>

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

protected:

	void DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) override;

private:
	bool m_is_update_needed = false;

	std::unordered_set< Actor* > m_cache_children = {};
	std::map< float, std::vector< Actor* > > m_cache_children_by_zindex;

	void AddCacheChildToZIndexSet( Actor* gl_actor, const float zindex );
	void RemoveCacheChildFromZIndexSet( Actor* gl_actor, const float zindex );

	types::Vec2< size_t > m_top_left = {};
	types::Vec2< size_t > m_bottom_right = {};
	types::texture::Texture* m_texture = nullptr;
	types::mesh::Rectangle* m_mesh = nullptr;
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	GLuint m_ibo_size = 0;

	size_t m_update_counter = 0;

private:
	friend class Actor;
	void UpdateCacheImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr, const bool force = false );

};

}
}
