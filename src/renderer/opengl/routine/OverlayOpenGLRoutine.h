#pragma once

#include "OpenGLRoutine.h"

#include "../shader_program/OrthographicOpenGLShaderProgram.h"

namespace renderer {
namespace opengl {
namespace routine {

MINOR_CLASS( Overlay, OpenGLRoutine )
	OverlayOpenGLRoutine( shader_program::OrthographicOpenGLShaderProgram *shader_program ) : m_shader_program( shader_program ) {};

	bool SceneBelongs( const scene::Scene *scene ) const;

	void Start();
	void Stop();
	void Iterate();

protected:
	shader_program::OrthographicOpenGLShaderProgram *m_shader_program;
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
