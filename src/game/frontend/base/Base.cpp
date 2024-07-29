#include "Base.h"

#include "game/frontend/Game.h"
#include "game/frontend/faction/Faction.h"
#include "game/frontend/tile/Tile.h"
#include "util/String.h"
#include "game/backend/base/Base.h"
#include "game/frontend/Slot.h"
#include "game/frontend/sprite/Sprite.h"
#include "game/frontend/sprite/InstancedSprite.h"
#include "game/frontend/text/InstancedText.h"
#include "scene/actor/Instanced.h"
#include "types/mesh/Rectangle.h"
#include "scene/actor/Sprite.h"
#include "types/texture/Texture.h"
#include "BaseManager.h"
#include "SlotBadges.h"

namespace game {
namespace frontend {
namespace base {

static const std::vector< uint8_t > s_base_render_population_thresholds = {
	1,
	4,
	8,
	15,
};

Base::Base(
	BaseManager* bm,
	const size_t id,
	Slot* slot,
	tile::Tile* tile,
	const bool is_owned,
	const types::Vec3& render_coords,
	text::InstancedText* render_name_sprite,
	size_t population
)
	: TileObject( tile )
	, m_bm( bm )
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
	, m_population( population )
	, m_is_guarded( !m_tile->GetUnits().empty() )
	, m_slot_badges( m_bm->GetSlotBadges( slot->GetIndex() ) ) {
	m_render_data.base = GetMeshTex( GetSprite()->instanced_sprite );
	m_render.badge.def = m_slot_badges->GetBaseBadgeSprite( m_population, m_is_guarded );
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

		ShowBadge();

		m_render.is_rendered = true;
	}
}

void Base::Hide() {
	if ( m_render.is_rendered ) {
		GetSprite()->instanced_sprite->actor->RemoveInstance( m_render.instance_id );
		m_render.name_sprite->Hide();
		HideBadge();
		m_render.is_rendered = false;
	}
}

void Base::Update() {
	const auto is_guarded = !m_tile->GetUnits().empty();
	if ( is_guarded != m_is_guarded ) {
		if ( m_render.is_rendered ) {
			HideBadge();
			m_render.badge.instance_id = 0;
		}
		m_is_guarded = is_guarded;
		m_render.badge.def = m_slot_badges->GetBaseBadgeSprite( m_population, m_is_guarded );
		if ( m_render.is_rendered ) {
			ShowBadge();
		}
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

void Base::ShowBadge() {
	if ( !m_render.badge.instance_id ) {
		m_render.badge.instance_id = m_render.badge.def->next_instance_id++;
	}
	m_render.badge.def->instanced_sprite->actor->SetInstance( m_render.badge.instance_id, SlotBadges::GetBadgeCoords( m_render.coords ) );
}

void Base::HideBadge() {
	m_render.badge.def->instanced_sprite->actor->RemoveInstance( m_render.badge.instance_id );
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
