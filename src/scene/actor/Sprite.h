#pragma once

#include "Actor.h"

#include "Types.h"
#include "types/mesh/Types.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Render;
}
}

namespace scene {
namespace actor {

CLASS( Sprite, Actor )

	Sprite( const std::string& name, const scene::actor::sprite_coords_t dimensions, types::texture::Texture* texture, const types::mesh::tex_coords_t tex_coords );
	Sprite( Sprite* orig );

	const scene::actor::sprite_coords_t& GetDimensions() const;
	types::texture::Texture* GetTexture() const;
	const types::mesh::tex_coords_t& GetTexCoords() const;
	const types::mesh::Render* GenerateMesh() const;

protected:
	const types::Vec2< float > m_dimensions;
	types::texture::Texture* m_texture = nullptr;
	const types::mesh::tex_coords_t m_tex_coords;

};

}
}
