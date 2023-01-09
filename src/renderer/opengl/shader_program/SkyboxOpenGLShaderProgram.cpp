#include "SkyboxOpenGLShaderProgram.h"

namespace renderer {
namespace opengl {
namespace shader_program {

void SkyboxOpenGLShaderProgram::AddShaders() {
	this->AddShader( GL_VERTEX_SHADER, "#version 130 \n\
\
in vec3 aVertex; \
out vec3 texCoord0; \
uniform mat4 uPVM; \
\
void main() { \
    gl_Position = (uPVM * vec4(aVertex, 1.0)).xyww; \
    texCoord0 = aVertex; \
} \
\
");

	this->AddShader( GL_FRAGMENT_SHADER, "#version 130 \n\
\
in vec3 texCoord0; \
out vec4 FragColor; \
uniform samplerCube uCubemap; \
\
void main (void) { \
    FragColor = texture(uCubemap, texCoord0); \
} \
\
");

};

void SkyboxOpenGLShaderProgram::Initialize() {
	m_gl_uniforms.pvm = this->GetUniformLocation( "uPVM" );
	m_gl_uniforms.cubemap = this->GetUniformLocation( "uCubemap" );
	m_gl_attributes.vertex = this->GetAttributeLocation("aVertex");
};

void SkyboxOpenGLShaderProgram::EnableAttributes() const {
	glEnableVertexAttribArray( m_gl_attributes.vertex );
	glVertexAttribPointer( m_gl_attributes.vertex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
};

void SkyboxOpenGLShaderProgram::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.vertex );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */
