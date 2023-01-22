#include "Font.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Font::AddShaders() {
	this->AddShader( GL_VERTEX_SHADER, "#version 130 \n\
\
in vec4 aCoord; \
out vec2 aTexCoord; \
uniform float uZIndex; \
\
void main(void) { \
	gl_Position = vec4(aCoord.xy, uZIndex, 1); \
	aTexCoord = vec2(aCoord.zw); \
} \
\
");

	this->AddShader( GL_FRAGMENT_SHADER, "#version 130 \n\
\
in vec2 aTexCoord; \
uniform sampler2D uTexture; \
uniform vec4 uColor; \
out vec4 oFragColor; \
\
void main(void) { \
	oFragColor = vec4(1, 1, 1, texture2D(uTexture, aTexCoord).r) * uColor; \
} \
\
");

}

void Font::Initialize() {
	m_gl_uniforms.texture = this->GetUniformLocation( "uTexture" );
	m_gl_uniforms.color = this->GetUniformLocation( "uColor" );
	m_gl_uniforms.z_index = this->GetUniformLocation( "uZIndex" );
	m_gl_attributes.coord = this->GetAttributeLocation( "aCoord" );
};

void Font::EnableAttributes() const {
	glEnableVertexAttribArray( m_gl_attributes.coord );
	glVertexAttribPointer( m_gl_attributes.coord, 4, GL_FLOAT, GL_FALSE, 0, 0 );
};

void Font::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.coord );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
