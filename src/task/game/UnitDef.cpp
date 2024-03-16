#include "UnitDef.h"

#include "game/unit/SpriteRender.h"

#include "engine/Engine.h"
#include "util/String.h"

namespace task {
namespace game {

UnitDef::UnitDef( InstancedSpriteManager* ism, const ::game::unit::Def* unitdef )
	: m_ism( ism )
	, m_id( unitdef->m_id )
	, m_name( unitdef->m_name )
	, m_type( unitdef->m_type ) {

	switch ( unitdef->m_type ) {
		case ::game::unit::Def::DT_STATIC: {
			const auto* def = (::game::unit::StaticDef*)unitdef;

			switch ( def->m_render->m_type ) {

				case ::game::unit::Render::RT_SPRITE: {
					const auto* render = (::game::unit::SpriteRender*)def->m_render;

					static_.movement_type = def->m_movement_type;
					static_.movement_per_turn = def->m_movement_per_turn;
					static_.render.is_sprite = true;

					const auto name = "Unit_" + def->m_id;
					auto* texture = g_engine->GetTextureLoader()->LoadTexture( render->m_file );
					const ::game::map::Consts::pcx_texture_coordinates_t& src_wh = {
						render->m_w,
						render->m_h,
					};
					const Vec2< float >& dst_wh = {
						::game::map::s_consts.tile.scale.x,
						::game::map::s_consts.tile.scale.y * ::game::map::s_consts.sprite.y_scale
					};
					const auto zindex = 0.5f;

					static_.render.morale_based_xshift = render->m_morale_based_xshift;
					if ( static_.render.morale_based_xshift ) {
						NEW( static_.render.morale_based_sprites, morale_based_sprites_t );
						for ( ::game::unit::Unit::morale_t morale = ::game::unit::Unit::MORALE_MIN ; morale <= ::game::unit::Unit::MORALE_MAX ; morale++ ) {
							const uint32_t xshift = static_.render.morale_based_xshift * ( morale - ::game::unit::Unit::MORALE_MIN );
							static_.render.morale_based_sprites->insert(
								{
									morale,
									{
										m_ism->GetInstancedSprite(
											name + "_" + std::to_string( morale ), texture, {
												render->m_x + xshift,
												render->m_y,
											},
											src_wh,
											{
												render->m_cx + xshift,
												render->m_cy,
											},
											dst_wh,
											zindex
										),
									}
								}
							);
						}
					}
					else {
						static_.render.sprite.instanced_sprite = m_ism->GetInstancedSprite(
							name, texture, {
								render->m_x,
								render->m_y,
							},
							src_wh,
							{
								render->m_cx,
								render->m_cy,
							},
							dst_wh,
							zindex
						);
					}
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
	if ( m_type == ::game::unit::Def::DT_STATIC ) {
		if ( static_.render.morale_based_xshift ) {
			DELETE( static_.render.morale_based_sprites );
		}
	}
}

const bool UnitDef::IsArtillery() const {
	return m_id != "SporeLauncher";
}

Sprite* UnitDef::GetSprite( const ::game::unit::Unit::morale_t morale ) {
	ASSERT_NOLOG( m_type == ::game::unit::Def::DT_STATIC, "only static units are supported for now" );
	ASSERT_NOLOG( static_.render.is_sprite, "only sprite unitdefs are supported for now" );

	auto& render = static_.render;
	ASSERT_NOLOG( render.is_sprite, "only sprite units are supported for now" );
	return render.morale_based_xshift
		? &render.morale_based_sprites->at( morale )
		: &render.sprite;
}

const bool UnitDef::IsImmovable() const {
	return static_.movement_type == ::game::unit::StaticDef::MT_IMMOVABLE;
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

}
}
