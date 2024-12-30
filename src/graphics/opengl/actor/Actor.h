#pragma once

#include "common/Common.h"

#include "types/Vec3.h"

namespace scene {
namespace actor {
class Actor;
}
class Camera;
}

namespace graphics {
namespace opengl {

class OpenGL;

class Cache;

namespace shader_program {
class ShaderProgram;
}

CLASS( Actor, common::Class )

	enum type_t {
		AT_NONE,
		AT_MESH,
		AT_SPRITE,
		AT_TEXT,
		AT_CACHE,
	};

	Actor( const type_t type, OpenGL* opengl, scene::actor::Actor* actor );
	~Actor();

	virtual void LoadMesh() {};
	virtual void LoadTexture() {};
	virtual void UnloadMesh() {};
	virtual void UnloadTexture() {};
	virtual bool MeshReloadNeeded() { return false; }
	virtual bool TextureReloadNeeded() { return false; }

	void Draw( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr );

	scene::actor::Actor* GetActor() const {
		return m_actor;
	}
	void UnlinkActor() {
		m_actor = nullptr;
	}
	const float GetZIndex() const;
	void SetZIndex( const float z_index );

	void SetCacheParent( Cache* cache_parent );

	virtual void OnWindowResize() {};

	const type_t m_type;

protected:
	friend class Cache;
	virtual void DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) = 0;

protected:

	OpenGL* m_opengl;
	scene::actor::Actor* m_actor;

	float m_z_index = 0.5f;

private:
	Cache* m_cache_parent = nullptr;

};

}
}
