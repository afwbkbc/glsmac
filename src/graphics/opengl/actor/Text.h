#pragma once

#include <GL/glew.h>

#include "Actor.h"

#include "types/Vec2.h"

namespace types {
class Font;
}

namespace scene::actor {
class Text;
}

namespace graphics {
namespace opengl {

class FontTexture;

CLASS( Text, Actor )

	Text( OpenGL* opengl, scene::actor::Text* actor, types::Font* font );
	~Text();

	void Update( types::Font* font, const std::string& text, const float x, const float y );
	void Draw( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) override;

protected:

	struct vertex_t {
		GLfloat x;
		GLfloat y;
		GLfloat z; // ignored
		GLfloat tx;
		GLfloat ty;
	};
	struct vertex_box_t {
		vertex_t v1;
		vertex_t v2;
		vertex_t v3;
		vertex_t v4;
	};
	types::Vec2< float > m_coords = {
		0,
		0
	};

	GLuint m_vbo = 0;
	size_t m_boxes_count = 0;

	types::Font* m_font = nullptr;
	std::string m_text = "";
	types::Vec2< size_t > m_last_window_size = {
		0,
		0
	};

	FontTexture* m_texture = nullptr;
};

}
}
