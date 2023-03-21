#include "Sprite.h"
#include "types/mesh/Render.h"

namespace scene {
namespace actor {

Sprite::Sprite( const std::string &name, const coords_t dimensions, types::Texture *texture, const types::mesh::Render::tex_coords_t tex_coords )
	: Actor( TYPE_SPRITE, name )
	, m_dimensions( dimensions )
	, m_texture( texture )
	, m_tex_coords( tex_coords )
{
	//
}

Sprite::Sprite( Sprite* orig )
	: Actor( TYPE_SPRITE, orig->GetLocalName() )
	, m_dimensions( orig->m_dimensions )
	, m_texture( orig->m_texture )
	, m_tex_coords( orig->m_tex_coords )
{
	//
}

const Sprite::coords_t& Sprite::GetDimensions() const {
	return m_dimensions;
}

types::Texture* Sprite::GetTexture() const {
	return m_texture;
}

const types::mesh::Render::tex_coords_t& Sprite::GetTexCoords() const {
	return m_tex_coords;
}

const types::mesh::Render* Sprite::GenerateMesh() const {
	auto* mesh = types::mesh::Render::Rectangle( m_dimensions.x, m_dimensions.y, m_tex_coords );
	mesh->UpdateAllNormals();
	return mesh;
}

}
}
