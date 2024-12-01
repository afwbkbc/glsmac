#pragma once

#include <GL/glew.h>

#include "Actor.h"

#include "scene/actor/Types.h"
#include "types/mesh/Types.h"

namespace types::texture {
class Texture;
}

namespace scene::actor {
class Sprite;
}

namespace graphics {
namespace opengl {

CLASS( Sprite, Actor )

	Sprite( OpenGL* opengl, scene::actor::Actor* actor );
	~Sprite();

	bool MeshReloadNeeded() override;
	bool TextureReloadNeeded() override;

	void LoadMesh() override;
	void LoadTexture() override;

	void Draw( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) override;

protected:

	scene::actor::Sprite* GetSpriteActor() const;

	scene::actor::sprite_coords_t m_last_dimensions = {
		0.0f,
		0.0f
	};
	const types::texture::Texture* m_last_texture = nullptr;
	types::mesh::tex_coords_t m_last_tex_coords = {
		{ 0.0f, 0.0f },
		{ 0.0f, 0.0f }
	};

	GLuint m_vbo = 0;
	GLuint m_ibo = 0;
	GLuint m_ibo_size = 0;

};

}
}
