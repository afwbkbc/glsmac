#include "AnimationDef.h"

#include "engine/Engine.h"
#include "game/backend/animation/FramesRow.h"
#include "loader/sound/SoundLoader.h"
#include "loader/texture/TextureLoader.h"
#include "task/game/sprite/InstancedSpriteManager.h"

namespace task {
namespace game {

AnimationDef::AnimationDef( sprite::InstancedSpriteManager* ism, const ::game::backend::animation::Def* def )
	: m_ism( ism )
	, m_id( def->m_id )
	, m_type( def->m_type )
	, m_duration_ms( def->m_duration_ms )
	, m_sound_file( def->m_sound_file ) {

	ASSERT_NOLOG( def->m_type == ::game::backend::animation::animation_type_t::AT_FRAMES_ROW, "only frames row animations are supported for now" );

	const auto* d = (::game::backend::animation::FramesRow*)def;
	m_render = d->m_render;
	ASSERT_NOLOG( m_render.frames_count > 0, "animation has no frames defined" );
}

AnimationDef::~AnimationDef() {
	//
}

const instanced_sprites_t& AnimationDef::GetSprites() {
	if ( m_sprites.empty() ) {
		auto* texture = g_engine->GetTextureLoader()->LoadCustomTexture( m_render.file );
		const uint32_t w = m_render.frame_width;
		const uint32_t h = m_render.frame_height;
		const uint32_t p = m_render.frame_padding;
		const types::Vec2< uint32_t > src_wh = {
			w,
			h
		};
		const types::Vec2< float > dst_wh = {
			m_render.scale_x,
			m_render.scale_y / 0.8f
		};
		const types::Vec2< uint32_t > cxy = {
			m_render.frame_center_x,
			m_render.frame_center_y,
		};
		for ( size_t i = 0 ; i < m_render.frames_count ; i++ ) {
			const uint32_t x = m_render.row_x + ( i % m_render.frames_per_row ) * ( w + p );
			const uint32_t y = m_render.row_y + ( i / m_render.frames_per_row ) * ( h + p );
			m_sprites.push_back(
				m_ism->GetInstancedSprite(
					"Animation_" + m_id,
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
					ZL_ANIMATIONS
				)
			);
		}
	}
	return m_sprites;
}

const size_t AnimationDef::GetDurationMs() const {
	return m_duration_ms;
}

const types::Sound* AnimationDef::GetSound() {
	if ( !m_sound ) {
		m_sound = g_engine->GetSoundLoader()->LoadCustomSound( m_sound_file );
	}
	return m_sound;
}

}
}
