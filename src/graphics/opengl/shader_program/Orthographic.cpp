#include "Orthographic.h"

using namespace scene;

#include "graphics/Graphics.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Orthographic::AddShaders() {
	AddShader( GL_VERTEX_SHADER, "#version 130 \n\
\
in vec3 aCoord; \
in vec2 aTexCoord; \
in vec4 aTintColor; \
in vec3 aNormal; \
uniform mat4 uWorld; \
uniform uint uFlags; \
out vec2 texpos; \
out vec4 tintcolor; \
out vec3 fragpos; \
out vec3 normal; \
\
void main(void) { \
	if ( " + S_HasFlag( "uFlags", actor::Mesh::RF_IGNORE_CAMERA ) + " ) { \
		gl_Position = vec4( aCoord, 1.0 ); \
	} \
	else { \
		gl_Position = uWorld * vec4( aCoord, 1.0 ); \
	} \
	texpos = vec2( aTexCoord.xy ); \
	tintcolor = aTintColor; \
	fragpos = aCoord; \
	normal = aNormal; \
} \
\
");
	AddShader( GL_FRAGMENT_SHADER, "#version 130 \n\
\
in vec2 texpos; \
in vec4 tintcolor; \
in vec3 fragpos; \
in vec3 normal; \
uniform sampler2D uTexture; \
uniform vec3 uLightPos; \
uniform vec4 uLightColor; \
uniform uint uFlags; \
uniform vec4 uTintColor; \
out vec4 FragColor; \
\
void main(void) { \
    vec3 ambient = uLightColor.rgb * ( 1.0 - uLightColor.a ); \
	vec3 lightdir = normalize( uLightPos ); \
	float diff = max( dot(normal, lightdir), 0.0 ); \
	vec3 diffuse = diff * uLightColor.rgb * uLightColor.a; \
	vec4 tex = texture2D( uTexture, vec2( texpos.xy ) ); \
	float gamma = 1.4; /* TODO: pass via uniform */ \
	vec3 color = vec3( tex.r * tintcolor.r, tex.g * tintcolor.g, tex.b * tintcolor.b ); \
	float alpha = tintcolor.a * tex.a; \
	if ( " + S_HasFlag( "uFlags", actor::Mesh::RF_USE_TINT ) + " ) { \
		color *= uTintColor.rgb; \
		alpha *= uTintColor.a; \
	} \
	if ( ! " + S_HasFlag( "uFlags", actor::Mesh::RF_IGNORE_LIGHTING ) + " ) { \
		color *= ambient + diffuse; \
	} \
	FragColor = vec4( color * gamma, alpha ); \
} \
\
");
}

void Orthographic::Initialize() {
	attributes.tex_coord = GetAttributeLocation( "aTexCoord" );
	attributes.coord = GetAttributeLocation( "aCoord" );
	attributes.tint_color = GetAttributeLocation( "aTintColor" );
	attributes.normal = GetAttributeLocation( "aNormal" );
	uniforms.texture = GetUniformLocation( "uTexture" );
	uniforms.light_pos = GetUniformLocation( "uLightPos" );
	uniforms.light_color = GetUniformLocation( "uLightColor" );
	uniforms.world = GetUniformLocation("uWorld");
	uniforms.flags = GetUniformLocation("uFlags");
	uniforms.tint_color = GetUniformLocation("uTintColor");
	
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
	glEnableVertexAttribArray( attributes.tint_color );
	glVertexAttribPointer( attributes.tint_color, types::mesh::Render::VERTEX_TINT_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Render::VERTEX_TINT_SIZE * tsz;
	glEnableVertexAttribArray( attributes.normal );
	glVertexAttribPointer( attributes.normal, types::mesh::Render::VERTEX_NORMAL_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid *)vaofs );
	vaofs += types::mesh::Render::VERTEX_NORMAL_SIZE * tsz;
};

void Orthographic::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
	glDisableVertexAttribArray( attributes.tex_coord );
	glDisableVertexAttribArray( attributes.tint_color );
	glDisableVertexAttribArray( attributes.normal );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
