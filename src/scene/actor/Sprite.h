#pragma once

#include "Actor.h"

#include "types/Texture.h"
#include "types/mesh/Render.h"

namespace scene {
namespace actor {

CLASS( Sprite, Actor )
	
	typedef types::Vec2< float > coords_t;
	typedef std::pair< coords_t, coords_t > tex_coords_t;
	Sprite( const std::string &name, const coords_t dimensions, const types::Texture *texture, const tex_coords_t tex_coords );
	Sprite( Sprite* orig );
	
	const coords_t& GetDimensions() const;
	const types::Texture* GetTexture() const;
	const tex_coords_t& GetTexCoords() const;
	const types::mesh::Render* GenerateMesh() const;
	
protected:
	const types::Vec2< float > m_dimensions;
	const types::Texture* m_texture = nullptr;
	const tex_coords_t m_tex_coords;
	
};
	
}
}
