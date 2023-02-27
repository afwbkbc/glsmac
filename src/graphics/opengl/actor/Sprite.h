#pragma once

#include <GL/glew.h>

#include "Actor.h"
#include "scene/actor/Sprite.h"

namespace graphics {
namespace opengl {

CLASS(Sprite, Actor)
	
	Sprite( scene::actor::Actor *actor );
	~Sprite();
	
	bool MeshReloadNeeded();
	bool TextureReloadNeeded();
	
	void LoadMesh();
	void LoadTexture();
	
	void Draw( shader_program::ShaderProgram *shader_program, Camera *camera = nullptr );
	
protected:
	
	scene::actor::Sprite* GetSpriteActor() const;
	
	actor::Sprite::coords_t m_last_dimensions = { 0.0f, 0.0f };
	std::string m_last_texture_name = "";
	actor::Sprite::tex_coords_t m_last_tex_coords = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
	
	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	GLuint m_ibo_size = 0;

};

} /* namespace opengl */
} /* namespace graphics */