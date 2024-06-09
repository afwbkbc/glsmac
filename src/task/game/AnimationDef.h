#pragma once

#include "InstancedSpriteManager.h"

#include "Types.h"
#include "game/animation/Types.h"

namespace types {
class Sound;
}

namespace game::animation {
class Def;
}

namespace task {
namespace game {

class AnimationDef {
public:

	AnimationDef( InstancedSpriteManager* ism, const ::game::animation::Def* def );
	~AnimationDef();

	const instanced_sprites_t& GetSprites();
	const size_t GetDurationMs() const;
	const types::Sound* GetSound();

private:

	InstancedSpriteManager* const m_ism = nullptr;

	::game::animation::sprite_render_info_t m_render = {};

	std::string m_id = "";
	::game::animation::animation_type_t m_type;

	types::Sound* m_sound = nullptr;

	instanced_sprites_t m_sprites = {};
	const size_t m_duration_ms = 0;
	const std::string m_sound_file = "";
};

}
}
