#include "World.h"

#include "engine/Engine.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void World::AddShaders() {
	AddShader( GL_VERTEX_SHADER, "#version 330 \n\
\
in vec3 aPosition; \
in vec2 aTexCoord; \
\
out vec2 TexCoord0; \
out vec3 posw;\
uniform mat4 uWorld; \
\
void main() \
{\
	gl_Position = vec4(uWorld * vec4(aPosition,1.0)); \
	posw = vec3( gl_Position );\
	TexCoord0 = vec2(aTexCoord); \
} \
");

	AddShader( GL_FRAGMENT_SHADER, "#version 330 \n\
const vec3 uFogColor = vec3(1., 1., 1.);\
uniform vec3 uCamPos;\
in vec2 TexCoord0; \
in vec3 posw;\
out vec4 FragColor; \
\
struct lightA {\
	vec3 Color;\
	float Intensity;\
};\
\
uniform lightA uLightA;\
uniform sampler2D uSampler; \
\
void main() \
{ \
	vec3 camFrag = posw - uCamPos;\
	vec3 FogBaseColor = vec3( 0.0, 0.0, 0.0 );\
	float distance = length(camFrag);\
	float a = 0.4;\
	float b = 0.0001;\
	float fogAmount = a * exp(-uCamPos.z*b) * ( 1.0-exp( -distance*camFrag.z*b ) ) / (b*camFrag.z);\
	vec3 TexBaseColor = vec3( 0.8, 0.6, 0.4 );\
	FragColor = vec4( mix( uLightA.Color * TexBaseColor * uLightA.Intensity, uLightA.Color * FogBaseColor, fogAmount ), 1.0 );\
} \
\
");
}

void World::Initialize() {
	uniforms.world = GetUniformLocation("uWorld");
	uniforms.light_color = GetUniformLocation( "uLightA.Color" );
	uniforms.light_intensity = GetUniformLocation( "uLightA.Intensity" );
	uniforms.campos = GetUniformLocation("uCamPos");
	//mUSampler=GetUniformLocation("uSampler");
	attributes.position = GetAttributeLocation("aPosition");
//	attributes.tex_coord = GetAttributeLocation("aTexCoord");
};

void World::EnableAttributes() const {
	glEnableVertexAttribArray( attributes.position );
	glVertexAttribPointer( attributes.position, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0 );
	//glEnableVertexAttribArray(mATexCoord);
	//glVertexAttribPointer( mATexCoord, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(OpenGLVec3));
};

void World::DisableAttributes() const {
	glDisableVertexAttribArray( attributes.position );
	//glDisableVertexAttribArray( attributes.tex_coord );
};

types::Matrix44 World::GetWorldMatrix() {
	types::Matrix44 projection_matrix, rotate_matrix, translate_matrix;

	//projection_matrix.ProjectionPerspective( g_engine->GetGraphics()->GetAspectRatio(), 90, 0.01, 100);

	types::Vec3 target, up;
	//CalculateTargetUp(angle,&target,&up);
	types::Vec3 axis(0.0f,1.0f,0.0f);
	types::Vec3 view(1.0f,0.0f,0.0f);
	view.Rotate( 45.0, axis );
	view.Normalize();
	axis = axis ^ view;
	axis.Normalize();
	view.Rotate( 0.0, axis );
	view.Normalize();

	up=view;
	target=view^axis;

	rotate_matrix.TransformCameraRotate( target, up );

	translate_matrix.TransformTranslate( -0.0 , 2.0, -0.0 );

	return projection_matrix * rotate_matrix * translate_matrix;
}

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
