#pragma once

#include <GL/glew.h>
#include <vector>

#include "Actor.h"

#include "scene/actor/Image.h"

namespace graphics {
namespace opengl {

CLASS( Image, Actor )
	
	Image( scene::actor::Image *actor );
	~Image();
	void LoadMesh();
	void LoadTexture();
	
	void Draw( shader_program::ShaderProgram *shader_program, Camera *camera = nullptr );
	
protected:
	//size_t m_update_timestamp = 0;

	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_ibo_size;
	
private:
	std::vector<float> m_vertex_data;
	std::vector<unsigned int> m_index_data;
};

} /* namespace opengl */
} /* namespace graphics */
