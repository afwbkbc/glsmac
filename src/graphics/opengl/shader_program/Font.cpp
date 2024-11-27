#include "Font.h"

#include "scene/actor/Actor.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Font::AddShaders() {
	AddShader(
		GL_VERTEX_SHADER, "#version 330 \n\
\
in vec4 aCoord; \
uniform uint uFlags; \
uniform vec2 uPosition; \
uniform float uZIndex; \
out vec2 texpos; \
out vec2 fragpos; \
\
void main(void) { \
	vec2 coord = aCoord.xy; \
	if ( " + S_HasFlag( "uFlags", scene::actor::Actor::RF_USE_2D_POSITION ) + " ) { \
		coord += uPosition; \
	}\
	gl_Position = vec4( coord, uZIndex, 1 ); \
	texpos = vec2( aCoord.zw ); \
	fragpos = coord; \
} \
\
"
	);

	AddShader(
		GL_FRAGMENT_SHADER, "#version 330 \n\
\
in vec2 texpos; \
in vec2 fragpos; \
uniform uint uFlags; \
uniform sampler2D uTexture; \
uniform vec4 uColor; \
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
	FragColor = vec4(1, 1, 1, texture2D(uTexture, texpos).r) * uColor; \
} \
\
"
	);

}

void Font::Initialize() {
	attributes.coord = GetAttributeLocation( "aCoord" );
	uniforms.flags = GetUniformLocation( "uFlags" );
	uniforms.position = GetUniformLocation( "uPosition" );
	uniforms.texture = GetUniformLocation( "uTexture" );
	uniforms.color = GetUniformLocation( "uColor" );
	uniforms.z_index = GetUniformLocation( "uZIndex" );
	uniforms.area_limits.min = GetUniformLocation( "uAreaLimitsMin" );
	uniforms.area_limits.max = GetUniformLocation( "uAreaLimitsMax" );
};

void Font::EnableAttributes() const {
	glEnableVertexAttribArray( attributes.coord );
	glVertexAttribPointer( attributes.coord, 4, GL_FLOAT, GL_FALSE, 0, 0 );
};

void Font::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
};

void Font::ClearFlags() {
	//glUniform1ui( uniforms.flags, 0 );
}

}
}
}
