#pragma once

#include "Routine.h"

#include "../shader_program/Simple2D.h"

#include "../FBO.h"

namespace graphics {
namespace opengl {
namespace routine {

CLASS( Overlay, Routine )
	Overlay( OpenGL* opengl, shader_program::Simple2D *shader_program );

	void Start();
	void Stop();
	void Iterate();

	void Redraw();
	
	bool SceneBelongs( const scene::Scene *scene ) const;

protected:
	
	bool m_is_redraw_needed = false;
	
	shader_program::Simple2D *m_shader_program;
	
	FBO* m_fbo = nullptr;
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
