#include "AnimationDef.h"

#include "engine/Engine.h"
#include "game/animation/FramesRow.h"

namespace task {
namespace game {

AnimationDef::AnimationDef( InstancedSpriteManager* ism, const ::game::animation::Def* def )
	: m_ism( ism )
	, m_id( def->m_id )
	, m_type( def->m_type ) {

	ASSERT_NOLOG( def->m_type == ::game::animation::Def::animation_type_t::AT_FRAMES_ROW, "only frames row animations are supported for now" );
	const auto* d = (::game::animation::FramesRow*)def;

	auto* texture = g_engine->GetTextureLoader()->LoadTexture( d->m_file );
	for ( size_t i = 0 ; i < d->m_frames_count ; i++ ) {
		const uint32_t x = d->m_row_x + i * ( d->m_frame_width + d->m_frame_padding );
		const uint32_t y = d->m_row_y;
		const uint32_t w = d->m_frame_width;
		const uint32_t h = d->m_frame_height;
		m_sprites.push_back(
			m_ism->GetInstancedSprite(
				"Animation_" + d->m_id,
				texture,
				{
					x,
					y
				},
				{
					w,
					h,
				},
				{
					x + w / 2,
					y + h / 2,
				},
				{ // ?
					1.0f,
					1.0f
				},
				0.5f // ?
			)
		);
	}

}

AnimationDef::~AnimationDef() {
	for ( const auto& it : m_sprites ) {
		DELETE( it );
	}
}

const AnimationDef::sprites_t& AnimationDef::GetSprites() const {
	return m_sprites;
}

}
}
