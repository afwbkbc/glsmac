#pragma once

#include <GL/glew.h>

#include "Actor.h"
#include "scene/actor/Mesh.h"

namespace graphics {
namespace opengl {

CLASS(Mesh, Actor)
	
	Mesh( scene::actor::Actor *actor );
	~Mesh();
	
	bool MeshReloadNeeded();
	bool DataMeshReloadNeeded();
	bool TextureReloadNeeded();
	void LoadMesh();
	void LoadTexture();
	
	void Draw( shader_program::ShaderProgram *shader_program, Camera *camera = nullptr );
	
	void OnWindowResize();
	
protected:
	
	scene::actor::Mesh* GetMeshActor() const;
	
	void PrepareDataMesh();
	
	size_t m_mesh_update_counter = 0;
	size_t m_data_mesh_update_counter = 0;
	const types::Texture* m_last_texture = nullptr;
	size_t m_last_texture_update_counter = 0;

	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	GLuint m_ibo_size = 0;

	struct {
		bool is_allocated = false;
		GLuint fbo = 0;
		GLuint picking_texture = 0;
		GLuint depth_texture = 0;
		GLuint vbo = 0;
		GLuint ibo = 0;
		GLuint ibo_size = 0;
		bool is_up_to_date = false; // reset on window resize or other events when it needs to be reloaded
	} m_data = {};
	
	mesh::Data::data_t GetDataAt( const size_t x, const size_t y );
	
};

} /* namespace opengl */
} /* namespace graphics */
