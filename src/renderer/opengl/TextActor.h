#pragma once

#include "Actor.h"

#include "scene/actor/TextActor.h"
#include "types/Font.h"

using namespace types;

namespace renderer {
namespace opengl {

class FontTexture;
	
class TextActor : public Actor {
public:
	TextActor( scene::actor::TextActor *actor, Font* font );
	~TextActor();
	
	void Update( Font* font, const string& text, const float x, const float y );
	void Draw( shader_program::OpenGLShaderProgram *shader_program );
	
protected:

	typedef struct {
		GLfloat x;
		GLfloat y;
		GLfloat tx;
		GLfloat ty;
	} vertex_t;
	typedef struct {
		vertex_t v1;
		vertex_t v2;
		vertex_t v3;
		vertex_t v4;
	} vertex_box_t;

	GLuint m_vbo = 0;
	size_t m_boxes_count = 0;

	Font *m_font = nullptr;
	FontTexture *m_texture = nullptr;
};

} /* namespace opengl */
} /* namespace renderer */

#include "FontTexture.h"
