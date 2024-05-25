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
class UnitDef;
class Slot;
class Tile;
class Sprite;
class BadgeDefs;
class InstancedSprite;

class Unit {
public:

	static constexpr size_t MOVE_DURATION_MS = 125;

	// TODO: refactor
	Unit(
		Game* game,
		BadgeDefs* badge_defs,
		const size_t id,
		UnitDef* def,
		Slot* slot,
		Tile* tile,
		const types::Vec3& render_coords,
		const bool is_owned,
		const ::game::unit::movement_t movement,
		const ::game::unit::morale_t morale,
		const std::string& morale_string,
		const ::game::unit::health_t health
	);
	~Unit();

	const size_t GetId() const;
	const bool IsOwned() const;
	const bool IsActive() const;
	Tile* GetTile() const;

	const size_t GetSelectionWeight() const;

	Sprite* GetSprite() const;
	Sprite* GetBadgeSprite() const;
	Sprite* GetBadgeHealthbarSprite() const;

	const std::string GetNameString() const;
	const std::string GetStatsString() const;
	const std::string& GetMoraleString() const;
	const std::string GetMovesString() const;

	void Iterate();

	void SetActiveOnTile();

	void Show();
	void Hide();

	const bool IsBadgeVisible() const;
	void ShowBadge();
	void HideBadge();

	void ShowFakeBadge( const uint8_t offset );
	void HideFakeBadge();

	void StartBadgeBlink();
	void StopBadgeBlink( const bool is_badge_shown );

	void Refresh();

	void SetMovement( const ::game::unit::movement_t movement );
	void SetHealth( const ::game::unit::health_t health );
	const bool CanMove() const;

	void SetTile( Tile* dst_tile );
	void MoveToTile( Tile* dst_tile );
	void UpdateFromTile();

	const bool IsValid() const;
	const bool IsMoving() const;

	struct meshtex_t {
		const types::mesh::Mesh* mesh = nullptr;
		types::texture::Texture* texture = nullptr;
	};
	struct render_data_t {
		meshtex_t unit;
		meshtex_t badge;
		meshtex_t healthbar;
	};
	const render_data_t& GetRenderData() const;

private:

	Game* m_game = nullptr;

	BadgeDefs* const m_badge_defs;

	size_t m_id = 0;
	UnitDef* m_def = nullptr;
	Slot* m_slot = nullptr;
	Tile* m_tile = nullptr;
	struct {
		types::Vec3 coords = {};
		bool is_rendered = false;
		size_t instance_id = 0;
		struct {
			Sprite* def = nullptr;
			size_t instance_id = 0;
			struct {
				Sprite* def = nullptr;
				size_t instance_id = 0;
			} healthbar;
			struct {
				util::Timer timer;
			} blink;
		} badge;
		struct {
			size_t instance_id = 0;
		} fake_badge;
	} m_render;

	const bool m_is_owned = false;
	bool m_is_active = false;
	::game::unit::movement_t m_movement = 0.0f;
	::game::unit::morale_t m_morale = 0;
	std::string m_morale_string = "";
	::game::unit::health_t m_health = 0;

	bool m_need_refresh = true;
	uint8_t m_fake_badge_offset = 0;

	const bool ShouldBeActive() const;

	render_data_t m_render_data = {};

	util::Scroller< types::Vec3 > m_mover;

	Unit::meshtex_t GetMeshTex( const InstancedSprite* sprite );
	void SetRenderCoords( const types::Vec3& coords );
};

}
}
