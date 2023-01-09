#pragma once

#include "scene/Scene.h"
#include "scene/actor/Actor.h"
#include "../Scene.h"
#include "../Actor.h"

#include "base/Module.h"

namespace renderer {

class OpenGLRenderer;

namespace opengl {
namespace routine {

MAJOR_CLASS( OpenGLRoutine, base::Module )
	bool AddScene( scene::Scene *scene );
	bool RemoveScene( scene::Scene *scene );
	~OpenGLRoutine();
	virtual opengl::Actor *AddCustomActor( scene::actor::Actor *actor ) {
		return NULL;
	}
	virtual void OnSceneRemove( Scene *scene ) {};
protected:
	friend class renderer::OpenGLRenderer;

	std::vector<scene::Scene *> m_scenes;
	std::vector< opengl::Scene *> m_gl_scenes;
	virtual bool SceneBelongs( const scene::Scene *scene ) const = 0;
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */

#include "renderer/OpenGLRenderer.h"
