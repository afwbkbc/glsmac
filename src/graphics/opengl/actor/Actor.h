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
class FBO;

namespace shader_program {
class ShaderProgram;
}

CLASS( Actor, common::Class )

	Actor( OpenGL* opengl, scene::actor::Actor* actor );
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

	virtual void OnWindowResize() {};

protected:
	virtual void DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) = 0;

	scene::actor::Actor* m_actor;

	float m_z_index = 0.0f;

	OpenGL* m_opengl;
	
private:
	bool m_is_redraw_needed = true;
	FBO* m_fbo = nullptr;
};

}
}
