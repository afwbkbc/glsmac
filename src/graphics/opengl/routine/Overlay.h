#pragma once

#include "Routine.h"

namespace graphics {
namespace opengl {

namespace shader_program {
class Simple2D;
class Font;
}

class FBO;

namespace routine {

CLASS( Overlay, Routine )
	Overlay( OpenGL* opengl, shader_program::Simple2D* shader_program, shader_program::Font* shader_program_font );

	void Start() override;
	void Stop() override;
	void Iterate() override;

	opengl::Actor* AddCustomActor( scene::actor::Actor* actor ) override;

	void Redraw();

	bool SceneBelongs( const scene::Scene* scene ) const override;

protected:

	bool m_is_redraw_needed = false;

	shader_program::Simple2D* m_shader_program = nullptr;
	shader_program::Font* m_shader_program_font = nullptr;

	FBO* m_fbo = nullptr;
};

}
}
}
