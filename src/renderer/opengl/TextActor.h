#pragma once

#include "Actor.h"

#include "scene/actor/TextActor.h"
#include "Font.h"

namespace renderer {
namespace opengl {

class TextActor : public Actor {
public:
	TextActor( scene::actor::TextActor *actor, Font *gl_font );
	~TextActor();
	void Draw( shader_program::OpenGLShaderProgram *shader_program );
protected:
	Font *m_gl_font;

};

} /* namespace opengl */
} /* namespace renderer */
