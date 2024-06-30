#include "Sprite.h"
#include "types/mesh/Render.h"

namespace scene {
namespace actor {

Sprite::Sprite(
	const std::string& name,
	const scene::actor::sprite_coords_t dimensions,
	types::texture::Texture* texture,
	const types::mesh::tex_coords_t tex_coords,
	const types::Vec2< types::mesh::tex_coord_t > dst_offsets
)
	: Actor( TYPE_SPRITE, name )
	, m_dimensions( dimensions )
	, m_texture( texture )
	, m_tex_coords( tex_coords )
	, m_dst_offsets( dst_offsets ) {
	//
}

Sprite::Sprite( Sprite* orig )
	: Actor( TYPE_SPRITE, orig->GetLocalName() )
	, m_dimensions( orig->m_dimensions )
	, m_texture( orig->m_texture )
	, m_tex_coords( orig->m_tex_coords ) {
	//
}

const scene::actor::sprite_coords_t& Sprite::GetDimensions() const {
	return m_dimensions;
}

types::texture::Texture* Sprite::GetTexture() const {
	return m_texture;
}

const types::mesh::tex_coords_t& Sprite::GetTexCoords() const {
	return m_tex_coords;
}

const types::Vec2< types::mesh::tex_coord_t >& Sprite::GetDstOffsets() const {
	return m_dst_offsets;
}

const types::mesh::Render* Sprite::GenerateMesh() const {
	auto* mesh = types::mesh::Render::Rectangle( m_dimensions.x, m_dimensions.y, m_tex_coords );
	mesh->UpdateAllNormals();
	return mesh;
}

const types::Vec3 Sprite::NormalizePosition( const types::Vec3& position ) const {
	return {
		position.x + m_dst_offsets.x,
		position.y + m_dst_offsets.y,
		position.z
	};
}

}
}
