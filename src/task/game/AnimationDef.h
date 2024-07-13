#pragma once

#include "Types.h"
#include "game/backend/animation/Types.h"

namespace types {
class Sound;
}

namespace game::backend::animation {
class Def;
}

namespace task {
namespace game {

namespace sprite {
class InstancedSpriteManager;
}

class AnimationDef {
public:

	AnimationDef( sprite::InstancedSpriteManager* ism, const ::game::backend::animation::Def* def );
	~AnimationDef();

	const instanced_sprites_t& GetSprites();
	const size_t GetDurationMs() const;
	const types::Sound* GetSound();

private:

	sprite::InstancedSpriteManager* const m_ism = nullptr;

	::game::backend::animation::sprite_render_info_t m_render = {};

	std::string m_id = "";
	::game::backend::animation::animation_type_t m_type;

	types::Sound* m_sound = nullptr;

	instanced_sprites_t m_sprites = {};
	const size_t m_duration_ms = 0;
	const std::string m_sound_file = "";
};

}
}
