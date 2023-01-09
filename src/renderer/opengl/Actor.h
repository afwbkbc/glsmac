#pragma once

#include "base/Base.h"

#include "scene/actor/Actor.h"
#include "shader_program/OpenGLShaderProgram.h"

namespace renderer {
namespace opengl {

MAJOR_CLASS( Actor, base::Base )
	Actor( scene::actor::Actor *actor );
	~Actor();
	virtual void Load() {};
	virtual void Unload() {};
	virtual bool ReloadNeeded() { return false; }
	virtual void Draw( shader_program::OpenGLShaderProgram *shader_program ) = 0;
	scene::actor::Actor *GetActor() const {
		return m_actor;
	}
protected:
	scene::actor::Actor *m_actor;
};

} /* namespace opengl */
} /* namespace renderer */
