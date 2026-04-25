#pragma once

#include <functional>
#include <GL/glew.h>

#include "common/Common.h"
#include "types/Vec2.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Render;
}
}

namespace graphics {
namespace opengl {

class OpenGL;

namespace shader_program {
class Orthographic;
}

CLASS( FBO, common::Class )

	FBO( OpenGL* opengl, const size_t width, const size_t height );
	~FBO();

	void Resize( size_t width, size_t height );

	void WriteBegin();
	void Write( const std::function< void() >& f );
	void WriteEnd();

	void Draw( shader_program::Orthographic* sp );

	void CaptureToTexture( types::texture::Texture* const texture, const types::Vec2< size_t >& top_left, const types::Vec2< size_t >& bottom_right );
	types::texture::Texture* CaptureToTexture();

protected:

	bool m_is_enabled = false;

	size_t m_width = 0;
	size_t m_height = 0;

	GLuint m_fbo = 0;
	struct {
		GLuint render = 0;
		GLuint depth = 0;
	} m_textures;

	// for rendering texture to screen
	GLuint m_vbo;
	GLuint m_ibo;
	size_t m_ibo_size;
	types::mesh::Render* m_mesh = nullptr;

private:
	OpenGL* m_opengl;

};

}
}
