#pragma once

#include <cstddef>

#include "game/frontend/TileObject.h"

#include "game/backend/unit/Types.h"

#include "util/Timer.h"
#include "util/Scroller.h"
#include "types/Vec3.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Simple;
}
}

namespace game {
namespace frontend {

class Game;
class Slot;

namespace sprite {
class Sprite;
class InstancedSprite;
}

namespace text {
class InstancedText;
}

namespace tile {
class Tile;
}

namespace faction {
class Faction;
}

namespace base {

class BaseManager;
class SlotBadges;

class Base : public TileObject {
public:

	Base(
		BaseManager* bm,
		const size_t id,
		const std::string& name,
		Slot* slot,
		tile::Tile* tile,
		const bool is_owned,
		const types::Vec3& render_coords,
		text::InstancedText* render_name_sprite
	);
	~Base();

	const size_t GetId() const;
	const std::string& GetName() const;
	faction::Faction* const GetFaction() const;
	const bool IsOwned() const;
	tile::Tile* GetTile() const;
	const size_t GetPopulation() const;

	sprite::Sprite* GetSprite() const;

	void Show();
	void Hide();
	void Update();

	struct meshtex_t {
		const types::mesh::Simple* mesh = nullptr;
		types::texture::Texture* texture = nullptr;
	};
	struct render_data_t {
		meshtex_t base;
		meshtex_t badge;
	};
	const render_data_t& GetRenderData() const;

	void* CreateOnBottomBarList( ui::ObjectsListItem* element ) const override;
	void DestroyOnBottomBarList( ui::ObjectsListItem* element, void* state ) const override;
	void* CreateOnBottomBarPreview( ui::ObjectPreview* element ) const override;
	void DestroyOnBottomBarPreview( ui::ObjectPreview* element, void* state ) const override;
	const bool OnBottomBarListActivate( Game* game ) override;

protected:
	void SetRenderCoords( const types::Vec3& coords ) override;

private:

	BaseManager* m_bm = nullptr;
	SlotBadges* const m_slot_badges;

	size_t m_id = 0;
	std::string m_name = "";

	faction::Faction* m_faction = nullptr;

	struct {
		types::Vec3 coords = {};
		sprite::Sprite* sprite = nullptr;
		text::InstancedText* name_sprite = nullptr;
		bool is_rendered = false;
		size_t instance_id = 0;
		struct {
			sprite::Sprite* def = nullptr;
			size_t instance_id = 0;
			text::InstancedText* label = nullptr;
		} badge;
	} m_render;

	bool m_is_guarded = false;

	const bool m_is_owned = false;

	render_data_t m_render_data = {};

	void ShowBadge();
	void HideBadge();

	meshtex_t GetMeshTex( const sprite::InstancedSprite* sprite );
};

}
}
}
