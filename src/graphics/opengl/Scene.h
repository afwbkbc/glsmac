#pragma once

#include <vector>
#include <map>
#include <set>

#include "base/Base.h"
#include "base/ObjectLink.h"
#include "scene/Scene.h"
#include "shader_program/OpenGLShaderProgram.h"
#include "Texture.h"
#include "Actor.h"

using namespace std;

namespace graphics {
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
	
	vector<base::ObjectLink *> m_gl_actors;
	map< float, set< Actor* > > m_gl_actors_by_zindex;
	
	
private:
	void RemoveActor( base::ObjectLink *link );
	void AddActorToZIndexSet( Actor* gl_actor );
	void RemoveActorFromZIndexSet( Actor* gl_actor );
};

} /* namespace opengl */
} /* namespace graphics */

#include "routine/OpenGLRoutine.h"

