#pragma once

#include <string>
#include <unordered_map>

#include "game/backend/unit/Types.h"

#include "types/Vec3.h"
// TODO: remove?
#include "game/frontend/sprite/Sprite.h"

namespace types {
namespace texture {
class Texture;
}
}

namespace game::backend::unit {
class Def;
}

namespace game {
namespace frontend {

namespace sprite {
class InstancedSpriteManager;
}

namespace unit {

class UnitDef {
public:
	UnitDef( sprite::InstancedSpriteManager* ism, const backend::unit::Def* unitdef );
	~UnitDef();

	const bool IsArtillery() const;

	sprite::Sprite* GetSprite( const backend::unit::morale_t morale );

	const bool IsImmovable() const;

	const std::string GetNameString() const;
	const std::string GetStatsString() const;

private:

	sprite::InstancedSpriteManager* const m_ism;

	backend::unit::sprite_render_info_t m_render = {};

	std::string m_id;
	std::string m_name;
	backend::unit::def_type_t m_type;

	typedef std::unordered_map< backend::unit::morale_t, sprite::Sprite > morale_based_sprites_t;

	struct {
		backend::unit::movement_type_t movement_type;
		backend::unit::movement_t movement_per_turn;
		struct {
			bool is_sprite = false;
			types::texture::Texture* texture = nullptr;
			sprite::Sprite sprite = {};
			morale_based_sprites_t* morale_based_sprites = nullptr;
		} render = {};
	} static_ = {};

	types::texture::Texture* GetSpriteTexture();
};

}
}
}
