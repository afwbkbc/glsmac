#include "Orthographic.h"

using namespace scene;

#include "graphics/Graphics.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Orthographic::AddShaders() {
	this->AddShader( GL_VERTEX_SHADER, "#version 140 \n\
\
in vec3 aCoord; \
in vec2 aTexCoord; \
in vec4 aTint; \
in vec3 aNormal; \
uniform mat4 uWorld[" + to_string( Graphics::MAX_WORLD_INSTANCES ) + "]; \
out vec2 texpos; \
out vec4 tint; \
out vec3 fragpos; \
out vec3 normal; \
\
void main(void) { \
	gl_Position = uWorld[ gl_InstanceID ] * vec4( aCoord, 1.0 ); \
	texpos = vec2( aTexCoord.xy ); \
	tint = aTint; \
	fragpos = aCoord; \
	normal = aNormal; \
} \
\
");
	this->AddShader( GL_FRAGMENT_SHADER, "#version 140 \n\
\
in vec2 texpos; \
in vec4 tint; \
in vec3 fragpos; \
in vec3 normal; \
uniform sampler2D uTexture; \
uniform vec3 uLightPos; \
uniform vec4 uLightColor; \
out vec4 FragColor; \
\
void main(void) { \
    vec3 ambient = uLightColor.rgb * ( 1.0 - uLightColor.a ); \
	vec3 lightdir = normalize( uLightPos ); \
	float diff = max( dot(normal, lightdir), 0.0 ); \
	vec3 diffuse = diff * uLightColor.rgb * uLightColor.a; \
	vec4 tex = texture2D( uTexture, vec2( texpos.xy ) ); \
	float gamma = 1.4; /* TODO: pass via uniform */ \
	FragColor = vec4( vec3( tex.r * tint.r, tex.g * tint.g, tex.b * tint.b ) * ( ambient + diffuse ) * gamma, tint.a * tex.a ); \
} \
\
");
}

void Orthographic::Initialize() {
	m_gl_attributes.tex_coord = this->GetAttributeLocation( "aTexCoord" );
	m_gl_attributes.coord = this->GetAttributeLocation( "aCoord" );
	m_gl_attributes.tint = this->GetAttributeLocation( "aTint" );
	m_gl_attributes.normal = this->GetAttributeLocation( "aNormal" );
	m_gl_uniforms.texture = this->GetUniformLocation( "uTexture" );
	m_gl_uniforms.light_pos = this->GetUniformLocation( "uLightPos" );
	m_gl_uniforms.light_color = this->GetUniformLocation( "uLightColor" );
	m_gl_uniforms.world = this->GetUniformLocation("uWorld");
};

void Orthographic::EnableAttributes() const {
	const size_t tsz = sizeof( types::Mesh::coord_t );
	const size_t vasz = types::Mesh::VERTEX_SIZE * tsz;
	size_t vaofs = 0;
	glEnableVertexAttribArray( m_gl_attributes.coord );
	glVertexAttribPointer( m_gl_attributes.coord, types::Mesh::VERTEX_COORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::Mesh::VERTEX_COORD_SIZE * tsz;
	glEnableVertexAttribArray( m_gl_attributes.tex_coord );
	glVertexAttribPointer( m_gl_attributes.tex_coord, types::Mesh::VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::Mesh::VERTEX_TEXCOORD_SIZE * tsz;
	glEnableVertexAttribArray( m_gl_attributes.tint );
	glVertexAttribPointer( m_gl_attributes.tint, types::Mesh::VERTEX_TINT_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::Mesh::VERTEX_TINT_SIZE * tsz;
	glEnableVertexAttribArray( m_gl_attributes.normal );
	glVertexAttribPointer( m_gl_attributes.normal, types::Mesh::VERTEX_NORMAL_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::Mesh::VERTEX_NORMAL_SIZE * tsz;
};

void Orthographic::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.coord );
	glDisableVertexAttribArray( m_gl_attributes.tex_coord );
	glDisableVertexAttribArray( m_gl_attributes.tint );
	glDisableVertexAttribArray( m_gl_attributes.normal );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
