#pragma once

#include "Routine.h"

namespace graphics {
namespace opengl {

namespace shader_program {
class Simple2D;
}

namespace routine {

CLASS( UI, Routine )
	UI( OpenGL* opengl, shader_program::Simple2D* shader_program );

	void Start() override;
	void Stop() override;
	void Iterate() override;

	opengl::Actor* AddCustomActor( scene::actor::Actor* actor ) override;

	bool SceneBelongs( const scene::Scene* scene ) const override;

protected:

	shader_program::Simple2D* m_shader_program = nullptr;

};

}
}
}
