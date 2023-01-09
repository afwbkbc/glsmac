#pragma once

#include <GL/glew.h>

#include "base/Base.h"

#include "types/Texture.h"

namespace renderer {
namespace opengl {

MAJOR_CLASS( Texture, base::Base )
	Texture( types::Texture *texture );
	~Texture();
	virtual void Load();
	virtual void Enable();
	virtual void Disable();
	virtual void Unload();
protected:
	bool m_loaded = false;

	GLenum m_target = GL_TEXTURE_2D;

	types::Texture *m_texture;

	GLuint m_texture_obj;
};

} /* namespace opengl */
} /* namespace renderer */
