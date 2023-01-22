#pragma once

#include <GL/glew.h>
#include <vector>

#include "Actor.h"

#include "scene/actor/Image.h"

using namespace std;

namespace graphics {
namespace opengl {

CLASS( Image, Actor )
	Image( scene::actor::Image *actor );
	~Image();
	bool ReloadNeeded();
	void Load();
	void Unload();
	void Draw( shader_program::ShaderProgram *shader_program );
protected:
	//size_t m_update_timestamp = 0;

	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_ibo_size;
	
private:
	vector<float> m_vertex_data;
	vector<unsigned int> m_index_data;
};

} /* namespace opengl */
} /* namespace graphics */
