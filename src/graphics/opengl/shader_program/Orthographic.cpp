#include "Orthographic.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Orthographic::AddShaders() {
	this->AddShader( GL_VERTEX_SHADER, "#version 130 \n\
\
in vec3 aCoord; \
in vec2 aTexCoord; \
in vec4 aTint; \
uniform mat4 uWorld; \
out vec2 texpos; \
out vec4 tint; \
\
void main(void) { \
	gl_Position = uWorld * vec4( aCoord, 1.0 ); \
	texpos = vec2(aTexCoord.xy); \
	tint = aTint; \
} \
\
");

	this->AddShader( GL_FRAGMENT_SHADER, "#version 130 \n\
\
in vec2 texpos; \
in vec4 tint; \
uniform sampler2D uTexture; \
out vec4 FragColor; \
\
void main(void) { \
	FragColor = vec4( texture2D(uTexture, vec2(texpos.xy)).xyz * tint.r, tint.a ); \
} \
\
");

}

void Orthographic::Initialize() {
	m_gl_attributes.tex_coord = this->GetAttributeLocation( "aTexCoord" );
	m_gl_attributes.coord = this->GetAttributeLocation( "aCoord" );
	m_gl_attributes.tint = this->GetAttributeLocation( "aTint" );
	m_gl_uniforms.texture = this->GetUniformLocation( "uTexture" );
	m_gl_uniforms.world = this->GetUniformLocation("uWorld");
};

void Orthographic::EnableAttributes() const {
	glEnableVertexAttribArray( m_gl_attributes.coord );
	glVertexAttribPointer( m_gl_attributes.coord, 3, GL_FLOAT, GL_FALSE, 36, (const GLvoid *)0 );
	glEnableVertexAttribArray( m_gl_attributes.tex_coord );
	glVertexAttribPointer( m_gl_attributes.tex_coord, 2, GL_FLOAT, GL_FALSE, 36, (const GLvoid *)12 );
	glEnableVertexAttribArray( m_gl_attributes.tint );
	glVertexAttribPointer( m_gl_attributes.tint, 4, GL_FLOAT, GL_FALSE, 36, (const GLvoid *)20 );
};

void Orthographic::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.coord );
	glDisableVertexAttribArray( m_gl_attributes.tex_coord );
	glDisableVertexAttribArray( m_gl_attributes.tint );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
