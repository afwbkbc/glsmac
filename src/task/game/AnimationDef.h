#pragma once

#include "InstancedSpriteManager.h"

#include "game/animation/Def.h"

#include "Sprite.h"

namespace task {
namespace game {

class AnimationDef {
public:
	typedef std::vector< InstancedSprite* > sprites_t;

	AnimationDef( InstancedSpriteManager* ism, const ::game::animation::Def* def );
	~AnimationDef();

	const sprites_t& GetSprites() const;

private:

	InstancedSpriteManager* const m_ism;

	std::string m_id;
	::game::animation::Def::animation_type_t m_type;

	sprites_t m_sprites = {};
};

}
}
