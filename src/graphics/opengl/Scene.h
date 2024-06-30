#pragma once

#include <vector>
#include <map>

#include "common/Common.h"
#include "actor/Actor.h"

namespace common {
class ObjectLink;
}

namespace scene {
class Scene;
}

namespace graphics {
namespace opengl {

class Texture;

namespace shader_program {
class ShaderProgram;
}

namespace routine {
class Routine;
}

CLASS( Scene, common::Class )
	Scene( scene::Scene* scene, routine::Routine* routine );
	~Scene();
	scene::Scene* GetScene() const;
	Texture* GetSkyboxTexture() const;
	common::ObjectLink* GetSkyboxTextureObj() const;
	void SetSkyboxTextureObj( common::ObjectLink* skybox_texture );
	// other_shader_program is used for fonts in Overlay routine // TODO: refactor
	void Draw( shader_program::ShaderProgram* shader_program, shader_program::ShaderProgram* other_shader_program = nullptr );
	void Update();

	void OnWindowResize();

protected:
	scene::Scene* m_scene;
	routine::Routine* m_routine;

	common::ObjectLink* m_skybox_texture = NULL;

	std::vector< common::ObjectLink* > m_gl_actors;
	std::map< float, std::vector< Actor* > > m_gl_actors_by_zindex;

private:
	void RemoveActor( common::ObjectLink* link );
	void AddActorToZIndexSet( Actor* gl_actor );
	void RemoveActorFromZIndexSet( Actor* gl_actor );
};

}
}
