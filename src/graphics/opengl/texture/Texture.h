#pragma once

#include <GL/glew.h>

#include "base/Base.h"

#include "types/Texture.h"

namespace graphics {
namespace opengl {

CLASS( Texture, base::Base )
	Texture( types::Texture* texture );
	~Texture();
	virtual void Load();
	virtual void Enable();
	virtual void Disable();
	virtual void Unload();
protected:

	types::Texture* m_texture;

	GLenum m_target = GL_TEXTURE_2D;
	GLuint m_texture_obj;

};

} /* namespace opengl */
} /* namespace graphics */
