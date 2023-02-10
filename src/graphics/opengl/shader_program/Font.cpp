#include "Font.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Font::AddShaders() {
	AddShader( GL_VERTEX_SHADER, "#version 130 \n\
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

	AddShader( GL_FRAGMENT_SHADER, "#version 130 \n\
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
	uniforms.texture = GetUniformLocation( "uTexture" );
	uniforms.color = GetUniformLocation( "uColor" );
	uniforms.z_index = GetUniformLocation( "uZIndex" );
	attributes.coord = GetAttributeLocation( "aCoord" );
};

void Font::EnableAttributes() const {
	glEnableVertexAttribArray( attributes.coord );
	glVertexAttribPointer( attributes.coord, 4, GL_FLOAT, GL_FALSE, 0, 0 );
};

void Font::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
