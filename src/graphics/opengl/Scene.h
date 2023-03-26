#pragma once

#include <vector>
#include <map>

#include "base/Base.h"
#include "base/ObjectLink.h"
#include "scene/Scene.h"
#include "shader_program/ShaderProgram.h"
#include "texture/Texture.h"
#include "actor/Actor.h"

namespace graphics {
namespace opengl {

namespace routine {
	class Routine;
}

CLASS( Scene, base::Base )
	Scene( scene::Scene *scene, routine::Routine *routine );
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
	// other_shader_program is used for fonts in Overlay routine // TODO: refactor
	void Draw( shader_program::ShaderProgram *shader_program, shader_program::ShaderProgram *other_shader_program = nullptr );
	void Update();
	
	void OnWindowResize();
	
protected:
	scene::Scene *m_scene;
	routine::Routine *m_routine;

	base::ObjectLink *m_skybox_texture = NULL;
	
	std::vector<base::ObjectLink *> m_gl_actors;
	std::map< float, std::vector< Actor* > > m_gl_actors_by_zindex;
	
	
private:
	void RemoveActor( base::ObjectLink *link );
	void AddActorToZIndexSet( Actor* gl_actor );
	void RemoveActorFromZIndexSet( Actor* gl_actor );
};

} /* namespace opengl */
} /* namespace graphics */

#include "routine/Routine.h"

