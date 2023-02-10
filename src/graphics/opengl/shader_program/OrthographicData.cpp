#include "OrthographicData.h"

using namespace scene;

#include "engine/Engine.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void OrthographicData::AddShaders() {
	AddShader( GL_VERTEX_SHADER, "#version 140 \n\
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
	AddShader( GL_FRAGMENT_SHADER, "#version 140 \n\
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
	attributes.coord = GetAttributeLocation( "aCoord" );
	attributes.data = GetAttributeLocation( "aData" );
	uniforms.world = GetUniformLocation("uWorld");
};

void OrthographicData::EnableAttributes() const {
	const size_t tsz = sizeof( types::mesh::Data::coord_t );
	const size_t vasz = types::mesh::Data::VERTEX_SIZE * tsz;
	size_t vaofs = 0;
	glEnableVertexAttribArray( attributes.coord );
	glVertexAttribPointer( attributes.coord, types::mesh::Data::VERTEX_COORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Data::VERTEX_COORD_SIZE * tsz;
	glEnableVertexAttribArray( attributes.data );
	glVertexAttribPointer( attributes.data, types::mesh::Data::VERTEX_DATA_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
};

void OrthographicData::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
	glDisableVertexAttribArray( attributes.data );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
