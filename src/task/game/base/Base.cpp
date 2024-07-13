#include "Base.h"

#include "task/game/Game.h"
#include "task/game/faction/Faction.h"
#include "task/game/tile/Tile.h"
#include "util/String.h"
#include "game/backend/base/Base.h"
#include "task/game/Slot.h"
#include "task/game/sprite/Sprite.h"
#include "task/game/sprite/InstancedSprite.h"
#include "task/game/text/InstancedText.h"
#include "scene/actor/Instanced.h"
#include "types/mesh/Rectangle.h"
#include "scene/actor/Sprite.h"
#include "types/texture/Texture.h"

namespace task {
namespace game {
namespace base {

static const std::vector< uint8_t > s_base_render_population_thresholds = {
	1,
	4,
	8,
	15,
};

Base::Base(
	const size_t id,
	Slot* slot,
	tile::Tile* tile,
	const bool is_owned,
	const types::Vec3& render_coords,
	text::InstancedText* render_name_sprite,
	size_t population
)
	: TileObject( tile )
	, m_id( id )
	, m_faction( slot->GetFaction() )
	, m_render(
		{
			render_coords,
			render_name_sprite,
			false,
			0,
		}
	)
	, m_is_owned( is_owned )
	, m_population( population ) {
	m_render_data.base = GetMeshTex( GetSprite()->instanced_sprite );
	m_tile->SetBase( this );
}

Base::~Base() {
	Hide();
	m_tile->UnsetBase( this );
	delete m_render.name_sprite;
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
	uint8_t size = 0;
	for ( uint8_t i = 0 ; i < s_base_render_population_thresholds.size() ; i++ ) {
		if ( s_base_render_population_thresholds.at( i ) > m_population ) {
			break;
		}
		size = i;
	}
	return m_faction->GetBaseSprite( m_tile->IsWater(), size, 0 ); // TODO: perimeter
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

		m_render.name_sprite->ShowAt(
			{
				m_render.coords.x,
				m_render.coords.y - 0.25f,
				m_render.coords.z - 0.25f
			}
		);

		m_render.is_rendered = true;
	}
}

void Base::Hide() {
	if ( m_render.is_rendered ) {
		GetSprite()->instanced_sprite->actor->RemoveInstance( m_render.instance_id );
		m_render.name_sprite->Hide();
		m_render.is_rendered = false;
	}
}

const Base::render_data_t& Base::GetRenderData() const {
	return m_render_data;
}

void Base::SetRenderCoords( const types::Vec3& coords ) {
	Hide();
	m_render.coords = coords;
	Show();
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
