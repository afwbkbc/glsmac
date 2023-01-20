#include "ImageActor.h"

#include "scene/actor/ImageActor.h"
#include "shader_program/OrthographicOpenGLShaderProgram.h"
#include "shader_program/WorldOpenGLShaderProgram.h"

#include "types/Matrix44.h"
#include "scene/actor/ImageActor.h"
#include "engine/Engine.h"

namespace graphics {
namespace opengl {

ImageActor::ImageActor( scene::actor::ImageActor *actor ) : Actor( actor ) {

	Log( "Creating OpenGL actor" );

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
	
	//m_update_timestamp = actor->GetImage()->UpdatedAt();
}

ImageActor::~ImageActor() {
	Log( "Destroying OpenGL actor" );

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );

}

bool ImageActor::ReloadNeeded() {
	// image can't change once it's loaded
	return false;
}

void ImageActor::Load() {
	Log( "Loading OpenGL actor" );
	
	
/*
	scene::actor::MeshActor *mesh_actor = (scene::actor::MeshActor *)m_actor;

	scene::mesh::Mesh *mesh = mesh_actor->GetMesh();
*/

	auto *actor = (scene::actor::ImageActor *)m_actor;
	
	Log("image width=" + to_string(actor->GetImage()->m_width) + " height=" + to_string(actor->GetImage()->m_height));
	Log("window width=" + to_string(g_engine->GetGraphics()->GetWindowWidth()) + " height=" + to_string(g_engine->GetGraphics()->GetWindowHeight()));
	
	// width and height scaled to viewport (to have same absolute pixel size always)
	float scaled_w = (float) actor->GetImage()->m_width / g_engine->GetGraphics()->GetWindowWidth();
	float scaled_h = (float) actor->GetImage()->m_height / g_engine->GetGraphics()->GetWindowHeight();
	
	m_vertex_data = { -scaled_w, -scaled_h, scaled_w, -scaled_h, scaled_w, scaled_h, -scaled_w, scaled_h };
	m_index_data = { 0, 1, 2, 3 };

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, m_vertex_data.size() * sizeof(float), (GLvoid *)m_vertex_data.data(), GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_index_data.size() * sizeof(unsigned int), (GLvoid *)m_index_data.data(), GL_STATIC_DRAW);

	m_ibo_size = m_index_data.size();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
 
}

void ImageActor::Unload() {
	Log( "Unloading OpenGL actor" );

	/*for (int i=0;i<this->mModel.mMaterials.size();i++)
		this->mGraphics->DeactivateTexture(&this->mModel.mMaterials[i]->mTexture1Map.texture,&this->mMaterialTextureObjs[i]);*/

}

void ImageActor::Draw( shader_program::OpenGLShaderProgram *shader_program ) {

	//Log( "Drawing" );

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );

	shader_program->Enable();

	switch ( shader_program->GetType() ) {
		case ( shader_program::OpenGLShaderProgram::TYPE_ORTHO ): {
			auto *ortho_shader_program = (shader_program::OrthographicOpenGLShaderProgram *)shader_program;
			//glUniform1f( ortho_shader_program->m_gl_uniforms.z_index, m_actor->GetPosition().z );
			//types::Color tint_color = ((scene::actor::MeshActor *)m_actor)->GetTintColor();
			/*types::Color tint_color = types::Color::WHITE();
			const GLfloat tint_color_data[4] = { tint_color.red, tint_color.green, tint_color.blue, tint_color.alpha };
			glUniform4fv( ortho_shader_program->m_gl_uniforms.tint, 1, tint_color_data );*/
			break;
		}
/*		case ( shader_program::OpenGLShaderProgram::TYPE_PERSP ): {
			auto *persp_shader_program = (shader_program::WorldOpenGLShaderProgram *)shader_program;

			types::Matrix44 matrix = m_actor->GetWorldMatrix();
			glUniformMatrix4fv( persp_shader_program->m_gl_uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));

			glUniform3f( persp_shader_program->m_gl_uniforms.light_color, 1.0, 1.0, 1.0 );
			glUniform1f( persp_shader_program->m_gl_uniforms.light_intensity, 1.0 );

		    //glUniform3f( persp_shader_program->m_gl_uniforms.campos, 0.0, 0.0, 0.0 );

			break;

		}*/
	}

	/*
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(MyVertex), BUFFER_OFFSET(12));   //The starting point of normals, 12 bytes away
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(MyVertex), BUFFER_OFFSET(24));   //The starting point of texcoords, 24 bytes away
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
*/

	//glEnableClientState( GL_VERTEX_ARRAY );
	//glVertexPointer( 2, GL_FLOAT, 0, (void *) 0 );
	// enable shader program?

	//glBindTexture(GL_TEXTURE_2D, this->mMaterialTextureObjs[0]);
	//glActiveTexture(GL_TEXTURE0);

/*
	math::Matrix44 matrix;

	matrix.Identity();
	matrix.ProjectionOrtho2D( 0.01f, 100.0f );

	glUniformMatrix4fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );*/

	//math::Vec2<> matrix( 0.0f, 0.0f );
	//glUniformMatrix2fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );

	//glUniformMatrix4fv(shader_program->mUWorld, 1, GL_TRUE, (const GLfloat*)(&this->mActorFinalMatrices[i]));

	glDrawElements( GL_QUADS, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );

	shader_program->Disable();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

} /* namespace opengl */
} /* namespace graphics */
