#pragma once

#include <GL/glew.h>

#include "common/Common.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Simple;
}
}

namespace graphics {
namespace opengl {

namespace shader_program {
class Simple2D;
}

CLASS( FBO, common::Class )

	// this doesn't seem to help with anything, keep it at 1 for now
	static constexpr ssize_t INTERNAL_RESOLUTION_MULTIPLIER = 1;

	FBO( const size_t width, const size_t height );
	~FBO();

	void Resize( size_t width, size_t height );

	void WriteBegin();
	void WriteEnd();

	void Draw( shader_program::Simple2D* sp );

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
	types::mesh::Simple* m_mesh = nullptr;
};

}
}
