#include "Font.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void Font::AddShaders() {
	AddShader( GL_VERTEX_SHADER, "#version 330 \n\
\
in vec4 aCoord; \
uniform vec2 uPosition; \
uniform float uZIndex; \
out vec2 texpos; \
out vec2 fragpos; \
\
void main(void) { \
	gl_Position = vec4( uPosition + aCoord.xy, uZIndex, 1 ); \
	texpos = vec2( aCoord.zw ); \
	fragpos = uPosition + aCoord.xy; \
} \
\
");

	AddShader( GL_FRAGMENT_SHADER, "#version 330 \n\
\
in vec2 texpos; \
in vec2 fragpos; \
uniform sampler2D uTexture; \
uniform vec4 uColor; \
uniform uint uFlags; \
uniform vec3 uCoordLimitsMin; \
uniform vec3 uCoordLimitsMax; \
out vec4 FragColor; \
\
void main(void) { \
	if ( " + S_HasFlag( "uFlags", actor::Actor::RF_USE_COORDINATE_LIMITS ) + " ) { \
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
	FragColor = vec4(1, 1, 1, texture2D(uTexture, texpos).r) * uColor; \
} \
\
");

}

void Font::Initialize() {
	attributes.coord = GetAttributeLocation( "aCoord" );
	uniforms.position = GetUniformLocation( "uPosition" );
	uniforms.texture = GetUniformLocation( "uTexture" );
	uniforms.color = GetUniformLocation( "uColor" );
	uniforms.z_index = GetUniformLocation( "uZIndex" );
	uniforms.flags = GetUniformLocation("uFlags");
	uniforms.coordinate_limits.min = GetUniformLocation( "uCoordLimitsMin" );
	uniforms.coordinate_limits.max = GetUniformLocation( "uCoordLimitsMax" );
};

void Font::EnableAttributes() const {
	glEnableVertexAttribArray( attributes.coord );
	glVertexAttribPointer( attributes.coord, 4, GL_FLOAT, GL_FALSE, 0, 0 );
};

void Font::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.coord );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
