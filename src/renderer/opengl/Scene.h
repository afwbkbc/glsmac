#pragma once

#include <vector>

#include "base/Base.h"
#include "base/ObjectLink.h"
#include "scene/Scene.h"
#include "shader_program/OpenGLShaderProgram.h"
#include "Texture.h"

namespace renderer {
namespace opengl {

namespace routine {
	class OpenGLRoutine;
}

MAJOR_CLASS( Scene, base::Base )
	Scene( scene::Scene *scene, routine::OpenGLRoutine *routine );
	~Scene();
	scene::Scene * GetScene() const {
		return m_scene;
	}
	Texture *GetSkyboxTexture() const {
		if ( ( !m_skybox_texture ) || m_skybox_texture->Removed() )
			return NULL;
		return m_skybox_texture->GetDstObject<Texture>();
	}
	base::ObjectLink *GetSkyboxTextureObj() const {
		return m_skybox_texture;
	}
	void SetSkyboxTextureObj( base::ObjectLink *skybox_texture ) {
		m_skybox_texture = skybox_texture;
	}
	void Draw( shader_program::OpenGLShaderProgram *shader_program );
	void Update();
protected:
	scene::Scene *m_scene;
	routine::OpenGLRoutine *m_routine;

	base::ObjectLink *m_skybox_texture = NULL;
	std::vector<base::ObjectLink *> m_gl_actors;
private:
	void RemoveActor( base::ObjectLink *link );
};

} /* namespace opengl */
} /* namespace renderer */

#include "routine/OpenGLRoutine.h"

