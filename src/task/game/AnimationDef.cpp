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
	, m_sound( g_engine->GetSoundLoader()->LoadSound( def->m_sound_file ) ) {

	ASSERT_NOLOG( def->m_type == ::game::animation::animation_type_t::AT_FRAMES_ROW, "only frames row animations are supported for now" );
	const auto* d = (::game::animation::FramesRow*)def;

	auto* texture = g_engine->GetTextureLoader()->LoadTexture( d->m_file );
	m_sprites = {};
	for ( size_t i = 0 ; i < d->m_frames_count ; i++ ) {
		const uint32_t x = d->m_row_x + i * ( d->m_frame_width + d->m_frame_padding );
		const uint32_t y = d->m_row_y;
		const uint32_t w = d->m_frame_width;
		const uint32_t h = d->m_frame_height;
		const auto cy = y + h / 2;

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
					x + (uint32_t)( (float)w / 2 * d->m_scale_x ),
					y + (uint32_t)( (float)h / ( 2.0f * d->m_scale_y / 0.8f ) ),
				},
				{
					d->m_scale_x,
					d->m_scale_y / 0.8f
				},
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
