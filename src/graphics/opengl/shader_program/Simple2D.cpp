#include "Simple2D.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Simple2D::AddShaders() {
	this->AddShader( GL_VERTEX_SHADER, "#version 130 \n\
\
in vec3 aCoord; \
in vec2 aTexCoord; \
out vec2 texpos; \
\
void main(void) { \
	gl_Position = vec4( aCoord, 1.0 ); \
	texpos = vec2(aTexCoord); \
} \
\
");

	this->AddShader( GL_FRAGMENT_SHADER, "#version 130 \n\
\
in vec2 texpos; \
uniform sampler2D uTexture; \
out vec4 FragColor; \
\
void main(void) { \
	FragColor = vec4(texture2D(uTexture, vec2(texpos.xy))); \
} \
\
");

}

void Simple2D::Initialize() {
	m_gl_attributes.tex_coord = this->GetAttributeLocation( "aTexCoord" );
	m_gl_attributes.coord = this->GetAttributeLocation( "aCoord" );
	m_gl_uniforms.texture = this->GetUniformLocation( "uTexture" );
};

void Simple2D::EnableAttributes() const {
	glEnableVertexAttribArray( m_gl_attributes.coord );
	glVertexAttribPointer( m_gl_attributes.coord, 3, GL_FLOAT, GL_FALSE, 20, (const GLvoid *)0 );
	glEnableVertexAttribArray( m_gl_attributes.tex_coord );
	glVertexAttribPointer( m_gl_attributes.tex_coord, 2, GL_FLOAT, GL_FALSE, 20, (const GLvoid *)12 );
};

void Simple2D::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.coord );
	glDisableVertexAttribArray( m_gl_attributes.tex_coord );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
