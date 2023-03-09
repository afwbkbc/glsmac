#include "Orthographic.h"

using namespace scene;

#include "graphics/Graphics.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Orthographic::AddShaders() {
	AddShader( GL_VERTEX_SHADER, "#version 330 \n\
\
in vec3 aCoord; \
in vec2 aTexCoord; \
in vec4 aTintColor; \
in vec3 aNormal; \
uniform mat4 uWorld[" + std::to_string( Graphics::MAX_WORLD_INSTANCES ) + "]; \
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
		gl_Position = uWorld[ gl_InstanceID ] * vec4( aCoord, 1.0 ); \
	} \
	texpos = vec2( aTexCoord.xy ); \
	tintcolor = aTintColor; \
	fragpos = aCoord; \
	normal = aNormal; \
} \
\
");
	AddShader( GL_FRAGMENT_SHADER, "#version 330 \n\
\
in vec2 texpos; \
in vec4 tintcolor; \
in vec3 fragpos; \
in vec3 normal; \
uniform sampler2D uTexture; \
uniform vec3 uLightPos[" + std::to_string( Graphics::MAX_WORLD_LIGHTS ) + "]; \
uniform vec4 uLightColor[" + std::to_string( Graphics::MAX_WORLD_LIGHTS ) + "]; \
uniform uint uFlags; \
uniform vec4 uTintColor; \
uniform vec3 uCoordLimitsMin; \
uniform vec3 uCoordLimitsMax; \
out vec4 FragColor; \
\
void main(void) { \
	if ( " + S_HasFlag( "uFlags", actor::Mesh::RF_USE_COORDINATE_LIMITS ) + " ) { \
		if ( \
			fragpos.x < uCoordLimitsMin.x || \
			fragpos.x > uCoordLimitsMax.x || \
			-fragpos.y < -uCoordLimitsMin.y || \
			-fragpos.y > -uCoordLimitsMax.y \
			/* TODO: fix Y inversion */ \
		) { \
			FragColor = vec4( 0.0, 0.0, 0.0, 0.0 ); \
			return; \
		} \
	} \
	vec3 ambient = vec3( 0.0, 0.0, 0.0 ); \
	vec3 diffuse = vec3( 0.0, 0.0, 0.0 ); \
	" + S_For( "i", 0, Graphics::MAX_WORLD_LIGHTS, " \
		ambient += uLightColor[ i ].rgb * ( 1.0 - uLightColor[ i ].a ); \
		vec3 lightdir = normalize( uLightPos[ i ] ); \
		float diff = max( dot(normal, lightdir), 0.0 ); \
		diffuse += diff * uLightColor[ i ].rgb * uLightColor[ i ].a; \
	" ) + " \
	ambient /= " + std::to_string( Graphics::MAX_WORLD_LIGHTS ) + "; \
	diffuse /= " + std::to_string( Graphics::MAX_WORLD_LIGHTS ) + "; \
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
	uniforms.coordinate_limits.min = GetUniformLocation( "uCoordLimitsMin" );
	uniforms.coordinate_limits.max = GetUniformLocation( "uCoordLimitsMax" );
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
