#include "AnimationDef.h"

#include "engine/Engine.h"
#include "game/animation/FramesRow.h"
#include "loader/sound/SoundLoader.h"
#include "loader/texture/TextureLoader.h"

namespace task {
namespace game {

AnimationDef::AnimationDef( InstancedSpriteManager* ism, const ::game::animation::Def* def )
	: m_ism( ism )
	, m_id( def->m_id )
	, m_type( def->m_type )
	, m_duration_ms( def->m_duration_ms )
	, m_sound( g_engine->GetSoundLoader()->LoadCustomSound( def->m_sound_file ) ) {

	ASSERT_NOLOG( def->m_type == ::game::animation::animation_type_t::AT_FRAMES_ROW, "only frames row animations are supported for now" );
	const auto* d = (::game::animation::FramesRow*)def;

	auto* texture = g_engine->GetTextureLoader()->LoadCustomTexture( d->m_file );
	m_sprites = {};
	const uint32_t w = d->m_frame_width;
	const uint32_t h = d->m_frame_height;
	const uint32_t p = d->m_frame_padding;
	const types::Vec2< uint32_t > src_wh = {
		w,
		h
	};
	const types::Vec2< float > dst_wh = {
		d->m_scale_x,
		d->m_scale_y / 0.8f
	};
	const types::Vec2< uint32_t > cxy = {
		d->m_frame_center_x,//(uint32_t)( (float)d->m_frame_center_x * d->m_scale_x ),
		d->m_frame_center_y,//(uint32_t)( (float)d->m_frame_center_y * d->m_scale_y * 0.8f ),
	};
	for ( size_t i = 0 ; i < d->m_frames_count ; i++ ) {
		const uint32_t x = d->m_row_x + ( i % d->m_frames_per_row ) * ( w + p );
		const uint32_t y = d->m_row_y + ( i / d->m_frames_per_row ) * ( h + p );

		m_sprites.push_back(
			m_ism->GetInstancedSprite(
				"Animation_" + d->m_id,
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
			)
		);
	}

}

AnimationDef::~AnimationDef() {
	//
}

const instanced_sprites_t& AnimationDef::GetSprites() const {
	return m_sprites;
}

const size_t AnimationDef::GetDurationMs() const {
	return m_duration_ms;
}

const types::Sound* AnimationDef::GetSound() const {
	return m_sound;
}

}
}
