#pragma once

#include <cstddef>

#include "game/unit/Types.h"

#include "util/Timer.h"
#include "util/Scroller.h"
#include "types/Vec3.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Mesh;
}
}

namespace task {
namespace game {

class Game;
class Slot;

namespace sprite {
class Sprite;
class InstancedSprite;
}

namespace tile {
class Tile;
}

namespace faction {
class Faction;
}

namespace base {

class BaseManager;

class Base {
public:

	// TODO: refactor
	Base(
		BaseManager* bm,
		const size_t id,
		Slot* slot,
		tile::Tile* tile,
		const types::Vec3& render_coords,
		const bool is_owned
	);
	~Base();

	const size_t GetId() const;
	const bool IsOwned() const;
	tile::Tile* GetTile() const;

	sprite::Sprite* GetSprite() const;

	void Show();
	void Hide();

	struct meshtex_t {
		const types::mesh::Mesh* mesh = nullptr;
		types::texture::Texture* texture = nullptr;
	};
	struct render_data_t {
		meshtex_t base;
	};
	const render_data_t& GetRenderData() const;

private:

	BaseManager* m_bm = nullptr;

	size_t m_id = 0;

	faction::Faction* m_faction = nullptr;

	tile::Tile* m_tile = nullptr;
	struct {
		types::Vec3 coords = {};
		bool is_rendered = false;
		size_t instance_id = 0;
	} m_render;

	const bool m_is_owned = false;

	render_data_t m_render_data = {};

	meshtex_t GetMeshTex( const sprite::InstancedSprite* sprite );
};

}
}
}
