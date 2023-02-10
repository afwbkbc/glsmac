#pragma once

#include <GL/glew.h>

#include "Actor.h"

#include "scene/actor/Mesh.h"

namespace graphics {
namespace opengl {

CLASS(Mesh, Actor)
	Mesh( scene::actor::Mesh *actor );
	~Mesh();
	bool ReloadNeeded();
	void Load();
	void Unload();
	void Draw( shader_program::ShaderProgram *shader_program, Camera *camera = nullptr );
	
	void OnResize();
	
protected:
	
	void PrepareDataMesh();
	
	size_t m_update_counter = 0;

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
		actor::Mesh::data_request_id_t last_processed_data_request_id = 0;
	} m_data = {};
	
	DataMesh::data_t GetDataAt( const size_t x, const size_t y );

};

} /* namespace opengl */
} /* namespace graphics */
