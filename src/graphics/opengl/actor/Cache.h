#pragma once

#include <GL/glew.h>
#include <unordered_set>
#include <map>

#include "Actor.h"

namespace graphics {
namespace opengl {

CLASS( Cache, Actor )

	Cache( OpenGL* opengl, scene::actor::Actor* actor );
	~Cache();

	void AddCacheChild( Actor* cache_child );
	void RemoveCacheChild( Actor* cache_child );
	void SetCacheChildZIndex( Actor* cache_child, const float zindex );

protected:

	void DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) override;

	struct vertex_t {
		GLfloat x;
		GLfloat y;
		GLfloat z; // ignored
		GLfloat tx;
		GLfloat ty;
	};
	struct vertex_box_t {
		vertex_t v1;
		vertex_t v2;
		vertex_t v3;
		vertex_t v4;
	};
	types::Vec2< float > m_coords = {
		0,
		0
	};

	GLuint m_vbo = 0;
	size_t m_boxes_count = 0;

	types::Vec2< size_t > m_last_window_size = {
		0,
		0
	};

	std::unordered_set< Actor* > m_cache_children = {};
	std::map< float, std::vector< Actor* > > m_cache_children_by_zindex;

	void AddCacheChildToZIndexSet( Actor* gl_actor, const float zindex );
	void RemoveCacheChildFromZIndexSet( Actor* gl_actor, const float zindex );
};

}
}
