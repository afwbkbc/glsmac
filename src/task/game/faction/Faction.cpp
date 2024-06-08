#include "Faction.h"

#include "game/rules/Faction.h"
#include "task/game/InstancedSpriteManager.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"

#include "scene/actor/Instanced.h"

namespace task {
namespace game {
namespace faction {

Faction::Faction( const ::game::rules::Faction* def, InstancedSpriteManager* ism )
	: m_border_color( def->m_colors.border )
	, m_is_progenitor( def->m_flags & ::game::rules::Faction::FF_PROGENITOR ) {

	auto* texture = g_engine->GetTextureLoader()->LoadCustomTexture( def->m_bases_render.file );

	const uint32_t w = def->m_bases_render.cell_width;
	const uint32_t h = def->m_bases_render.cell_height;
	const uint32_t p = def->m_bases_render.cell_padding;
	const types::Vec2< uint32_t > src_wh = {
		w,
		h
	};
	const types::Vec2< float > dst_wh = {
		def->m_bases_render.scale_x,
		def->m_bases_render.scale_y / 0.8f
	};
	const types::Vec2< uint32_t > cxy = {
		(uint32_t)def->m_bases_render.cell_cx,
		(uint32_t)def->m_bases_render.cell_cy
	};

	for ( uint8_t base_type = 0 ; base_type < 6 ; base_type++ ) {
		for ( uint8_t base_size = 0 ; base_size < 4 ; base_size++ ) {

			const uint32_t x = def->m_bases_render.grid_x + base_size * ( w + p );
			const uint32_t y = def->m_bases_render.grid_y + base_type * ( h + p );

			m_base_sprites[ base_type ][ base_size ] = {
				ism->GetInstancedSprite(
					"Faction_Base_" + def->m_id + "_" + std::to_string( base_type ) + "_" + std::to_string( base_size ),
					texture,
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
					0.5f // ?
				),
				1
			};
		}
	}
}

}
}
}
