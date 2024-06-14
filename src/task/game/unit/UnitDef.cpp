#include "UnitDef.h"

#include "game/unit/StaticDef.h"
#include "game/unit/SpriteRender.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "util/String.h"
#include "task/game/sprite/InstancedSpriteManager.h"
#include "game/map/Consts.h"

namespace task {
namespace game {
namespace unit {

UnitDef::UnitDef( sprite::InstancedSpriteManager* ism, const ::game::unit::Def* unitdef )
	: m_ism( ism )
	, m_id( unitdef->m_id )
	, m_name( unitdef->m_name )
	, m_type( unitdef->m_type ) {

	switch ( unitdef->m_type ) {
		case ::game::unit::DT_STATIC: {
			const auto* def = (::game::unit::StaticDef*)unitdef;

			switch ( def->m_render->m_type ) {

				case ::game::unit::Render::RT_SPRITE: {
					m_render = ( (::game::unit::SpriteRender*)def->m_render )->m_render;

					static_.movement_type = def->m_movement_type;
					static_.movement_per_turn = def->m_movement_per_turn;
					static_.render.is_sprite = true;

					break;
				}
				default:
					THROW( "unknown unit render type: " + std::to_string( def->m_render->m_type ) );
			}
			break;
		}
		default:
			THROW( "unknown unit def type: " + std::to_string( unitdef->m_type ) );
	}
}

UnitDef::~UnitDef() {
	if ( m_type == ::game::unit::DT_STATIC ) {
		if ( static_.render.morale_based_xshift ) {
			if ( static_.render.morale_based_sprites ) {
				DELETE( static_.render.morale_based_sprites );
			}
		}
	}
}

const bool UnitDef::IsArtillery() const {
	return m_id != "SporeLauncher";
}

sprite::Sprite* UnitDef::GetSprite( const ::game::unit::morale_t morale ) {
	ASSERT_NOLOG( m_type == ::game::unit::DT_STATIC, "only static units are supported for now" );
	ASSERT_NOLOG( static_.render.is_sprite, "only sprite unitdefs are supported for now" );

	if ( m_render.morale_based_xshift ) {
		if ( !static_.render.morale_based_sprites ) {
			NEW( static_.render.morale_based_sprites, morale_based_sprites_t );
		}
		auto it = static_.render.morale_based_sprites->find( morale );
		if ( it == static_.render.morale_based_sprites->end() ) {
			const uint32_t xshift = static_.render.morale_based_xshift * ( morale - ::game::unit::MORALE_MIN );
			it = static_.render.morale_based_sprites->insert(
				{
					morale,
					{
						m_ism->GetInstancedSprite(
							"Unit_" + m_id + "_" + std::to_string( morale ), GetSpriteTexture(), {
								m_render.x + xshift,
								m_render.y,
							},
							{
								m_render.w,
								m_render.h,
							},
							{
								m_render.cx + xshift,
								m_render.cy,
							},
							{
								::game::map::s_consts.tile.scale.x,
								::game::map::s_consts.tile.scale.y * ::game::map::s_consts.sprite.y_scale
							},
							ZL_UNITS
						),
					}
				}
			).first;
		}
		return &it->second;
	}
	else {
		if ( !static_.render.sprite.instanced_sprite ) {
			static_.render.sprite = {
				m_ism->GetInstancedSprite(
					"Unit_" + m_id, GetSpriteTexture(), {
						m_render.x,
						m_render.y,
					},
					{
						m_render.w,
						m_render.h,
					},
					{
						m_render.cx,
						m_render.cy,
					},
					{
						::game::map::s_consts.tile.scale.x,
						::game::map::s_consts.tile.scale.y * ::game::map::s_consts.sprite.y_scale
					},
					ZL_UNITS
				),
				1
			};
		}
		return &static_.render.sprite;
	}
}

const bool UnitDef::IsImmovable() const {
	return static_.movement_type == ::game::unit::MT_IMMOVABLE;
}

const std::string UnitDef::GetNameString() const {
	return m_name;
}

const std::string UnitDef::GetStatsString() const {
	std::string str = "";
	if ( m_id == "SporeLauncher" ) {
		str += "(?)";
	}
	else {
		str += "?";
	}
	return str + " - ? - " + util::String::ApproximateFloat( static_.movement_per_turn );

}

types::texture::Texture* UnitDef::GetSpriteTexture() {
	if ( !static_.render.texture ) {
		static_.render.texture = g_engine->GetTextureLoader()->LoadCustomTexture( m_render.file );
	}
	return static_.render.texture;
}

}
}
}
