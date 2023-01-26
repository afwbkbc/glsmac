#pragma once

#include "Routine.h"

#include "../shader_program/Simple2D.h"

namespace graphics {
namespace opengl {
namespace routine {

CLASS( Overlay, Routine )
	Overlay( shader_program::Simple2D *shader_program ) : m_shader_program( shader_program ) {};

	bool SceneBelongs( const scene::Scene *scene ) const;

	void Start();
	void Stop();
	void Iterate();

protected:
	shader_program::Simple2D *m_shader_program;
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
