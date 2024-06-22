#include "Faction.h"

#include "game/rules/Faction.h"
#include "task/game/sprite/InstancedSpriteManager.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

#include "scene/actor/Instanced.h"

namespace task {
namespace game {
namespace faction {

Faction::Faction( const ::game::rules::Faction* def, sprite::InstancedSpriteManager* ism )
	: m_ism( ism )
	, m_id( def->m_id )
	, m_colors(
		{
			def->m_colors.text,
			def->m_colors.border
		}
	)
	, m_is_progenitor( def->m_flags & ::game::rules::Faction::FF_PROGENITOR )
	, m_render(
		{
			def->m_bases_render,
		}
	) {
	//
}

sprite::Sprite* Faction::GetBaseSprite( const bool is_water, const uint8_t size, const uint8_t perimeter_level ) {
	ASSERT_NOLOG( size < 4, "base size overflow ( " + std::to_string( size ) + " >= 4 )" );
	ASSERT_NOLOG( perimeter_level < 3, "base perimeter level overflow ( " + std::to_string( perimeter_level ) + " >= 3 )" );
	const uint8_t index = ( is_water
		? 12
		: 0
	) + size + perimeter_level * 4;
	auto it = m_base_grid_sprites.find( index );
	if ( it == m_base_grid_sprites.end() ) {

		const uint32_t w = m_render.bases_render.cell_width;
		const uint32_t h = m_render.bases_render.cell_height;
		const uint32_t p = m_render.bases_render.cell_padding;
		const types::Vec2< uint32_t > src_wh = {
			w,
			h
		};
		const types::Vec2< float > dst_wh = {
			m_render.bases_render.scale_x,
			m_render.bases_render.scale_y / 1.0f
		};
		const types::Vec2< uint32_t > cxy = {
			(uint32_t)m_render.bases_render.cell_cx,
			(uint32_t)m_render.bases_render.cell_cy
		};

		const uint32_t x = m_render.bases_render.grid_x + size * ( w + p );
		const uint32_t y = m_render.bases_render.grid_y + ( perimeter_level + ( is_water
			? 3
			: 0
		)
		) * ( h + p );

		it = m_base_grid_sprites.insert(
			{
				index,
				{
					m_ism->GetInstancedSprite(
						"Faction_Base_" + m_id + "_" + ( is_water
							? "W"
							: "L"
						) + " " + std::to_string( size ) + "_" + std::to_string( perimeter_level ),
						GetBaseGridTexture(),
						{
							x,
							y
						},
						src_wh,
						{
							x + cxy.x,
							y + cxy.y
						},
						dst_wh,
						ZL_BASES
					),
					1
				}
			}
		).first;

	}
	return &it->second;
}

types::texture::Texture* Faction::GetBaseGridTexture() {
	if ( !m_base_grid_texture ) {
		m_base_grid_texture = g_engine->GetTextureLoader()->LoadCustomTexture( m_render.bases_render.file );
	}
	return m_base_grid_texture;
}

}
}
}
