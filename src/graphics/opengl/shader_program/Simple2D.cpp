#include "Simple2D.h"

#include "scene/actor/Actor.h"
#include "types/mesh/Simple.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Simple2D::AddShaders() {
	AddShader(
		GL_VERTEX_SHADER, "#version 330 \n\
\
in vec3 aCoord; \
in vec2 aTexCoord; \
uniform uint uFlags; \
uniform vec2 uPosition; \
out vec2 texpos; \
out vec3 fragpos; \
\
void main(void) { \
	vec3 coord = aCoord; \
	if ( " + S_HasFlag( "uFlags", scene::actor::Actor::RF_USE_2D_POSITION ) + " ) { \
		coord += vec3( uPosition, 0.0 ); \
	} \
	gl_Position = vec4( coord, 1.0 ); \
	texpos = vec2(aTexCoord); \
	fragpos = coord; \
} \
\
"
	);

	AddShader(
		GL_FRAGMENT_SHADER, "#version 330 \n\
\
in vec2 texpos; \
in vec3 fragpos; \
uniform uint uFlags; \
uniform vec4 uTintColor; \
uniform sampler2D uTexture; \
uniform vec3 uAreaLimitsMin; \
uniform vec3 uAreaLimitsMax; \
out vec4 FragColor; \
\
void main(void) { \
	if ( " + S_HasFlag( "uFlags", scene::actor::Actor::RF_USE_AREA_LIMITS ) + " ) { \
		if ( \
			fragpos.x < uAreaLimitsMin.x || \
			fragpos.x > uAreaLimitsMax.x || \
			-fragpos.y < -uAreaLimitsMin.y || \
			-fragpos.y > -uAreaLimitsMax.y \
			/* TODO: fix Y inversion */ \
		) { \
			FragColor = vec4( 0.0, 0.0, 0.0, 0.0 ); \
			return; \
		} \
	} \
	vec4 color = vec4(texture2D(uTexture, vec2(texpos.xy))); \
	if ( " + S_HasFlag( "uFlags", scene::actor::Actor::RF_USE_TINT ) + " ) { \
		color *= uTintColor; \
	} \
	FragColor = color; \
} \
\
"
	);

}

void Simple2D::Initialize() {
	attributes.tex_coord = GetAttributeLocation( "aTexCoord" );
	attributes.coord = GetAttributeLocation( "aCoord" );
	uniforms.flags = GetUniformLocation( "uFlags" );
	uniforms.tint_color = GetUniformLocation( "uTintColor" );
	uniforms.position = GetUniformLocation( "uPosition" );
	uniforms.texture = GetUniformLocation( "uTexture" );
	uniforms.area_limits.min = GetUniformLocation( "uAreaLimitsMin" );
	uniforms.area_limits.max = GetUniformLocation( "uAreaLimitsMax" );
};

void Simple2D::EnableAttributes() const {
	const size_t tsz = sizeof( types::mesh::coord_t );
	const size_t vasz = types::mesh::Simple::VERTEX_SIZE * tsz;
	size_t vaofs = 0;
	glEnableVertexAttribArray( attributes.coord );
	glVertexAttribPointer( attributes.coord, types::mesh::Simple::VERTEX_COORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid*)vaofs );
	vaofs += types::mesh::Simple::VERTEX_COORD_SIZE * tsz;
	glEnableVertexAttribArray( attributes.tex_coord );
	glVertexAttribPointer( attributes.tex_coord, types::mesh::Simple::VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE, vasz, (const GLvoid*)vaofs );
	vaofs += types::mesh::Simple::VERTEX_TEXCOORD_SIZE * tsz;
};

void Simple2D::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
	glDisableVertexAttribArray( attributes.tex_coord );
};

void Simple2D::ClearFlags() {
	glUniform1ui( uniforms.flags, 0 );
}

}
}
}
