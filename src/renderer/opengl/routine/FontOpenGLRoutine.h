#pragma once

#include <vector>

#include "OpenGLRoutine.h"

#include "../shader_program/FontOpenGLShaderProgram.h"
#include "base/ObjectLink.h"
#include "../Font.h"

namespace renderer {
namespace opengl {
namespace routine {

MINOR_CLASS( Font, OpenGLRoutine )
	FontOpenGLRoutine( shader_program::FontOpenGLShaderProgram *shader_program ) : m_shader_program( shader_program ) {};

	bool SceneBelongs( const scene::Scene *scene ) const;

	void Start();
	void Stop();
	void Iterate();

	opengl::Actor *AddCustomActor( scene::actor::Actor *actor );
protected:
	shader_program::FontOpenGLShaderProgram *m_shader_program;

	std::vector<base::ObjectLink *> m_gl_fonts;
private:
	void RemoveFont( base::ObjectLink *link );
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
