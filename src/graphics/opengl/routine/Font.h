#pragma once

#include <vector>

#include "Routine.h"

#include "../shader_program/Font.h"
#include "base/ObjectLink.h"
#include "../texture/FontTexture.h"

namespace graphics {
namespace opengl {
namespace routine {

CLASS( Font, Routine )
	
	Font( OpenGL* opengl, shader_program::Font *shader_program );

	bool SceneBelongs( const scene::Scene *scene ) const;

	void Start();
	void Stop();
	void Iterate();

	opengl::Actor *AddCustomActor( scene::actor::Actor *actor );
	
protected:
	shader_program::Font *m_shader_program;

	std::vector<base::ObjectLink *> m_gl_fonts;
	
private:
	void RemoveFont( base::ObjectLink *link );
};

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
