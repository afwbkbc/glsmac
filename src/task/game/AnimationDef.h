#pragma once

#include "InstancedSpriteManager.h"

#include "game/animation/Def.h"

#include "Sprite.h"
#include "types/Sound.h"

namespace task {
namespace game {

class AnimationDef {
public:
	typedef std::vector< InstancedSprite* > sprites_t;

	AnimationDef( InstancedSpriteManager* ism, const ::game::animation::Def* def );
	~AnimationDef();

	const sprites_t& GetSprites() const;
	const size_t GetDurationMs() const;
	const types::Sound* GetSound() const;

private:

	InstancedSpriteManager* const m_ism;

	std::string m_id;
	::game::animation::Def::animation_type_t m_type;

	types::Sound* m_sound;

	sprites_t m_sprites = {};
	const size_t m_duration_ms;
};

}
}
