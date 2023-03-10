#pragma once

#include "Actor.h"

#include "scene/actor/Text.h"
#include "types/Font.h"

using namespace types;

namespace graphics {
namespace opengl {

class FontTexture;
	
CLASS( Text, Actor )

	Text( scene::actor::Text *actor, Font* font );
	~Text();
	
	void Update( Font* font, const std::string& text, const float x, const float y );
	void Draw( shader_program::ShaderProgram *shader_program, Camera *camera = nullptr );
	
protected:

	struct vertex_t {
		GLfloat x;
		GLfloat y;
		GLfloat tx;
		GLfloat ty;
	};
	struct vertex_box_t {
		vertex_t v1;
		vertex_t v2;
		vertex_t v3;
		vertex_t v4;
	};
	Vec2< float > m_coords = { 0, 0 };

	GLuint m_vbo = 0;
	size_t m_boxes_count = 0;

	Font *m_font = nullptr;
	std::string m_text = "";
	FontTexture *m_texture = nullptr;
};

} /* namespace opengl */
} /* namespace graphics */

#include "../texture/FontTexture.h"
