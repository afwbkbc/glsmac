#pragma once

#include "Actor.h"

#include "game/map/tile/Types.h"

#include "util/Timer.h"

namespace task {
namespace game {
namespace actor {

CLASS( TileSelection, Actor )

	static constexpr uint16_t GLOW_STEP = 10; // ms
	static constexpr float GLOW_SPEED = 0.04f; // per step
	static constexpr float GLOW_MAX = 1.0f;
	static constexpr float GLOW_MIN = 0.25f;

	TileSelection( const ::game::map::tile::tile_vertices_t& coords );

	void Iterate() override;

private:
	types::texture::Texture* m_texture = nullptr;

	float m_glow = GLOW_MAX;
	int8_t m_glow_direction = -1;
	util::Timer m_glow_timer;

};

}
}
}
