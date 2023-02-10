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
uniform uint uFlags; \
uniform uint uTint; \
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
	attributes.tex_coord = this->GetAttributeLocation( "aTexCoord" );
	attributes.coord = this->GetAttributeLocation( "aCoord" );
	attributes.tint = this->GetAttributeLocation( "aTint" );
	attributes.normal = this->GetAttributeLocation( "aNormal" );
	uniforms.texture = this->GetUniformLocation( "uTexture" );
	uniforms.light_pos = this->GetUniformLocation( "uLightPos" );
	uniforms.light_color = this->GetUniformLocation( "uLightColor" );
	uniforms.world = this->GetUniformLocation("uWorld");
	
};

void Orthographic::EnableAttributes() const {
	const size_t tsz = sizeof( types::mesh::Mesh::coord_t );
	const size_t vasz = types::mesh::Render::VERTEX_SIZE * tsz;
	size_t vaofs = 0;
	glEnableVertexAttribArray( attributes.coord );
	glVertexAttribPointer( attributes.coord, types::mesh::Render::VERTEX_COORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Render::VERTEX_COORD_SIZE * tsz;
	glEnableVertexAttribArray( attributes.tex_coord );
	glVertexAttribPointer( attributes.tex_coord, types::mesh::Render::VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Render::VERTEX_TEXCOORD_SIZE * tsz;
	glEnableVertexAttribArray( attributes.tint );
	glVertexAttribPointer( attributes.tint, types::mesh::Render::VERTEX_TINT_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Render::VERTEX_TINT_SIZE * tsz;
	glEnableVertexAttribArray( attributes.normal );
	glVertexAttribPointer( attributes.normal, types::mesh::Render::VERTEX_NORMAL_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Render::VERTEX_NORMAL_SIZE * tsz;
};

void Orthographic::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
	glDisableVertexAttribArray( attributes.tex_coord );
	glDisableVertexAttribArray( attributes.tint );
	glDisableVertexAttribArray( attributes.normal );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
