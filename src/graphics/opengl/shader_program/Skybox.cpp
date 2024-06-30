#include "Skybox.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Skybox::AddShaders() {
	AddShader(
		GL_VERTEX_SHADER, "#version 330 \n\
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
"
	);

	AddShader(
		GL_FRAGMENT_SHADER, "#version 330 \n\
\
in vec3 texCoord0; \
out vec4 FragColor; \
uniform samplerCube uCubemap; \
\
void main (void) { \
    FragColor = texture(uCubemap, texCoord0); \
} \
\
"
	);

};

void Skybox::Initialize() {
	uniforms.pvm = GetUniformLocation( "uPVM" );
	uniforms.cubemap = GetUniformLocation( "uCubemap" );
	attributes.vertex = GetAttributeLocation( "aVertex" );
};

void Skybox::EnableAttributes() const {
	glEnableVertexAttribArray( attributes.vertex );
	glVertexAttribPointer( attributes.vertex, 3, GL_FLOAT, GL_FALSE, 0, 0 );
};

void Skybox::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.vertex );
};

}
}
}
