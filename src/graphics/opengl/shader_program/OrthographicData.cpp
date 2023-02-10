#include "OrthographicData.h"

using namespace scene;

#include "engine/Engine.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void OrthographicData::AddShaders() {
	this->AddShader( GL_VERTEX_SHADER, "#version 140 \n\
\
in vec3 aCoord; \
in uint aData; \
uniform mat4 uWorld[" + to_string( Graphics::MAX_WORLD_INSTANCES ) + "]; \
out float data; \
\
void main(void) { \
	gl_Position = uWorld[ gl_InstanceID ] * vec4( aCoord, 1.0 ); \
	data = aData; \
} \
\
");
	this->AddShader( GL_FRAGMENT_SHADER, "#version 140 \n\
\
in float data; \
out uint FragColor; \
\
void main(void) { \
	FragColor = uint( data ); \
} \
\
");
}

void OrthographicData::Initialize() {
	m_gl_attributes.coord = this->GetAttributeLocation( "aCoord" );
	m_gl_attributes.data = this->GetAttributeLocation( "aData" );
	m_gl_uniforms.world = this->GetUniformLocation("uWorld");
};

void OrthographicData::EnableAttributes() const {
	const size_t tsz = sizeof( types::mesh::Data::coord_t );
	const size_t vasz = types::mesh::Data::VERTEX_SIZE * tsz;
	size_t vaofs = 0;
	glEnableVertexAttribArray( m_gl_attributes.coord );
	glVertexAttribPointer( m_gl_attributes.coord, types::mesh::Data::VERTEX_COORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Data::VERTEX_COORD_SIZE * tsz;
	glEnableVertexAttribArray( m_gl_attributes.data );
	glVertexAttribPointer( m_gl_attributes.data, types::mesh::Data::VERTEX_DATA_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
};

void OrthographicData::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.coord );
	glDisableVertexAttribArray( m_gl_attributes.data );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
