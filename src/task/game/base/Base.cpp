#include "Base.h"

#include "task/game/Game.h"
#include "task/game/faction/Faction.h"
#include "task/game/tile/Tile.h"
#include "util/String.h"
#include "game/base/Base.h"
#include "task/game/Slot.h"
#include "task/game/sprite/Sprite.h"
#include "task/game/sprite/InstancedSprite.h"
#include "scene/actor/Instanced.h"
#include "types/mesh/Rectangle.h"
#include "scene/actor/Sprite.h"
#include "types/texture/Texture.h"

namespace task {
namespace game {
namespace base {

Base::Base(
	BaseManager* bm,
	const size_t id,
	Slot* slot,
	tile::Tile* tile,
	const types::Vec3& render_coords,
	const bool is_owned
)
	: m_bm( bm )
	, m_id( id )
	, m_faction( slot->GetFaction() )
	, m_tile( tile )
	, m_render(
		{
			render_coords,
			false,
			0,
		}
	)
	, m_is_owned( is_owned ) {
	m_render_data.base = GetMeshTex( GetSprite()->instanced_sprite );
	m_tile->SetBase( this );
}

Base::~Base() {
	Hide();
	m_tile->UnsetBase( this );
}

const size_t Base::GetId() const {
	return m_id;
}

const bool Base::IsOwned() const {
	return m_is_owned;
}

tile::Tile* Base::GetTile() const {
	return m_tile;
}

sprite::Sprite* Base::GetSprite() const {
	return m_faction->GetBaseSprite( false, 1, 0 );
}

void Base::Show() {
	if ( !m_render.is_rendered ) {
		const auto& c = m_render.coords;

		sprite::Sprite* sprite = GetSprite();

		if ( !m_render.instance_id ) {
			m_render.instance_id = sprite->next_instance_id++;
		}
		sprite->instanced_sprite->actor->SetInstance(
			m_render.instance_id, {
				c.x,
				c.y,
				c.z
			}
		);

		m_render.is_rendered = true;
	}
}

void Base::Hide() {
	if ( m_render.is_rendered ) {
		GetSprite()->instanced_sprite->actor->RemoveInstance( m_render.instance_id );
		m_render.is_rendered = false;
	}
}

const Base::render_data_t& Base::GetRenderData() const {
	return m_render_data;
}

Base::meshtex_t Base::GetMeshTex( const sprite::InstancedSprite* sprite ) {
	auto* texture = sprite->actor->GetSpriteActor()->GetTexture();
	NEWV( mesh, types::mesh::Rectangle );
	mesh->SetCoords(
		{
			0.0f,
			0.0f
		},
		{
			1.0f,
			1.0f
		},
		{
			sprite->xy.x,
			sprite->xy.y
		},
		{
			sprite->xy.x + sprite->wh.x,
			sprite->xy.y + sprite->wh.y
		},
		{
			texture->m_width,
			texture->m_height
		},
		0.8f
	);
	return {
		mesh,
		texture,
	};
}

}
}
}
