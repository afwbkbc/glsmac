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

class OpenGL;
class Texture;

namespace shader_program {
class ShaderProgram;
}

namespace routine {
class Routine;
}

CLASS( Scene, common::Class )
	Scene( OpenGL* opengl, scene::Scene* scene, routine::Routine* routine );
	~Scene();
	scene::Scene* GetScene() const;

	void Draw( shader_program::ShaderProgram* shader_program );
	void Update();

	void OnWindowResize();

protected:
	scene::Scene* m_scene;
	routine::Routine* m_routine;

	common::ObjectLink* m_skybox_texture = NULL;

	std::vector< common::ObjectLink* > m_gl_actors;
	std::map< float, std::vector< Actor* > > m_gl_actors_by_zindex;

private:
	OpenGL* m_opengl;

	void RemoveActor( common::ObjectLink* link );
	void AddActorToZIndexSet( Actor* gl_actor );
	void RemoveActorFromZIndexSet( Actor* gl_actor );
};

}
}
