#pragma once

#include "Routine.h"

namespace graphics {
namespace opengl {

namespace shader_program {
class Simple2D;
}

class FBO;

namespace routine {

CLASS( Overlay, Routine )
	Overlay( OpenGL* opengl, shader_program::Simple2D* shader_program );

	void Start() override;
	void Stop() override;
	void Iterate() override;

	opengl::Actor* AddCustomActor( scene::actor::Actor* actor ) override;

	void Redraw();

	bool SceneBelongs( const scene::Scene* scene ) const override;

protected:

	bool m_is_redraw_needed = false;

	shader_program::Simple2D* m_shader_program = nullptr;

	FBO* m_fbo = nullptr;
};

}
}
}
