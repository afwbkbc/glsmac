#include "Sprite.h"
#include "types/mesh/Render.h"

namespace scene {
namespace actor {

Sprite::Sprite( const std::string &name, const coords_t dimensions, const types::Texture *texture, const tex_coords_t tex_coords )
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

const types::Texture* Sprite::GetTexture() const {
	return m_texture;
}

const Sprite::tex_coords_t& Sprite::GetTexCoords() const {
	return m_tex_coords;
}

const types::mesh::Render* Sprite::GenerateMesh() const {
	const float wh = m_dimensions.x / 2;
	const float hh = m_dimensions.y / 2;
	
	NEWV( mesh, types::mesh::Render, 4, 2 );
	const auto top_left = mesh->AddVertex( { -wh, -hh, 0.0f }, { m_tex_coords.first.x, m_tex_coords.first.y } );
	const auto top_right = mesh->AddVertex( { wh, -hh, 0.0f }, { m_tex_coords.second.x, m_tex_coords.first.y } );
	const auto bottom_right = mesh->AddVertex( { wh, hh, 0.0f }, { m_tex_coords.second.x, m_tex_coords.second.y } );
	const auto bottom_left = mesh->AddVertex( { -wh, hh, 0.0f }, { m_tex_coords.first.x, m_tex_coords.second.y } );
	mesh->AddSurface( { bottom_right, bottom_left, top_left } );
	mesh->AddSurface( { top_left, top_right, bottom_right } );
	mesh->Finalize();
	
	return mesh;
}

}
}
