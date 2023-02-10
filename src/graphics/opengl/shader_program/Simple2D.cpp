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
	attributes.tex_coord = this->GetAttributeLocation( "aTexCoord" );
	attributes.coord = this->GetAttributeLocation( "aCoord" );
	uniforms.texture = this->GetUniformLocation( "uTexture" );
};

void Simple2D::EnableAttributes() const {
	const size_t tsz = sizeof( types::mesh::Mesh::coord_t );
	const size_t vasz = types::mesh::Simple::VERTEX_SIZE * tsz;
	size_t vaofs = 0;
	glEnableVertexAttribArray( attributes.coord );
	glVertexAttribPointer( attributes.coord, types::mesh::Simple::VERTEX_COORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Simple::VERTEX_COORD_SIZE * tsz;
	glEnableVertexAttribArray( attributes.tex_coord );
	glVertexAttribPointer( attributes.tex_coord, types::mesh::Simple::VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Simple::VERTEX_TEXCOORD_SIZE * tsz;
};

void Simple2D::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
	glDisableVertexAttribArray( attributes.tex_coord );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
