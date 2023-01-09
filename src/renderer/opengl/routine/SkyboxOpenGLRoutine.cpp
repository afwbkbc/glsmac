#include <algorithm>

#include "SkyboxOpenGLRoutine.h"

#include "engine/Engine.h"
#include "types/Matrix44.h"
#include "../CubemapTexture.h"

namespace renderer {
namespace opengl {
namespace routine {

void SkyboxOpenGLRoutine::Start() {

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );

/*
	glGenTextures( 1, &m_texture );
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_texture );
	uint16_t target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	//Texture *texture2d;
	for (uint8_t i=0;i<6;i++) { // experimental, we assume GL_TEXTURE_CUBE_MAP_* are sequential
		//texture2d=this->mTexture->mTextures[i];

		size_t width = 512;
		size_t height = 512;
		unsigned char bpp = 3;
		size_t sz = width * height * bpp;
		unsigned char texdata[ sz ];
		for ( int i = 0; i < sz ; i++ )
			texdata[i] = rand()%48;
		//memset( texdata, 100, sizeof( texdata ) );

		//glTexImage2D(target, 0, GL_RGB, texture2d->GetWidth(), texture2d->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture2d->GetRGBData());
		glTexImage2D( target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texdata );
		target++;
	};

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
*/
	// load vertices
	GLfloat cube_vertices[] = {
		-1.0,  1.0,  1.0,
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
	};

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW );
	GLushort cube_indices[] = {
		0, 1, 2, 3,
		3, 2, 6, 7,
		7, 6, 5, 4,
		4, 5, 1, 0,
		0, 3, 7, 4,
		2, 1, 5, 6,
	};

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW );
	m_ibo_size = sizeof(cube_indices) / sizeof(GLushort);
}

void SkyboxOpenGLRoutine::Stop() {
	//glDeleteTextures( 1, &m_texture );

	/*for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		auto *skybox_texture_obj = (*it)->GetSkyboxTextureObj();
		if ( skybox_texture_obj )
			RemoveTexture( skybox_texture_obj );
	}*/

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );
}

void SkyboxOpenGLRoutine::RemoveTexture( base::ObjectLink *link ) {
	if ( !link->Removed() )
		link->GetSrcObject<types::Texture>()->m_renderer_object = NULL;
	auto *gl_texture = link->GetDstObject<Texture>();
	gl_texture->Unload();
	delete gl_texture;
	delete link;
}

void SkyboxOpenGLRoutine::OnSceneRemove( Scene *scene ) {
	auto *skybox_texture = scene->GetSkyboxTextureObj();
	if ( skybox_texture )
		RemoveTexture( skybox_texture );
}

void SkyboxOpenGLRoutine::Iterate() {

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {

		// remove missing skybox texture
		auto *skybox_texture_obj = (*it)->GetSkyboxTextureObj();
		if ( skybox_texture_obj && skybox_texture_obj->Removed() ) {
			RemoveTexture(  skybox_texture_obj );
			(*it)->SetSkyboxTextureObj( NULL );
			skybox_texture_obj = NULL;
		}

		// create skybox texture
		if ( !skybox_texture_obj ) {
			auto *texture = (*it)->GetScene()->GetSkyboxTexture();
			if ( texture ) {
				auto *gl_texture = new CubemapTexture( texture );
				auto obj = new base::ObjectLink( texture, gl_texture );
				texture->m_renderer_object = obj;
				(*it)->SetSkyboxTextureObj( obj );
				gl_texture->Load();
			}
		}
	}

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		auto *camera = (*it)->GetScene()->GetCamera();
		if ( camera ) {

			auto *texture = (*it)->GetSkyboxTexture();
			if ( texture ) {

				glEnable( GL_CULL_FACE );

				glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );

				m_shader_program->Enable();

				glUniformMatrix4fv( m_shader_program->m_gl_uniforms.pvm, 1, GL_TRUE, (const GLfloat*)&camera->GetMatrix() );

				GLint OldCullFaceMode;
				glGetIntegerv( GL_CULL_FACE_MODE, &OldCullFaceMode );
				glCullFace( GL_FRONT );

				texture->Enable();
/*				glBindTexture( GL_TEXTURE_CUBE_MAP, m_texture );
				glActiveTexture( GL_TEXTURE0 );*/

				glDrawElements( GL_QUADS, m_ibo_size, GL_UNSIGNED_SHORT, 0 );

				texture->Disable();

				glCullFace( OldCullFaceMode );

				m_shader_program->Disable();

				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
				glBindBuffer( GL_ARRAY_BUFFER, 0 );

				glDisable( GL_CULL_FACE );
			}
		}
	}

}

bool SkyboxOpenGLRoutine::SceneBelongs( const scene::Scene *scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_PERSP;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
