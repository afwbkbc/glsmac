#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>

#include "game/backend/map/tile/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"

namespace types::mesh {
class Render;
}

namespace game::backend::map::tile {
class Tile;
class TileState;
}

namespace game {
namespace frontend {

namespace unit {
class Unit;
}

namespace base {
class Base;
}

namespace tile {

class Tile {
public:

	static std::vector< size_t > GetUnitsOrder( const std::unordered_map< size_t, unit::Unit* >& units );

	Tile( const types::Vec2< size_t >& coords );

	const bool IsWater() const;

	const types::Vec2< size_t >& GetCoords() const;

	void AddUnit( unit::Unit* unit );
	void RemoveUnit( unit::Unit* unit );
	void SetActiveUnit( unit::Unit* unit );

	void SetBase( base::Base* base );
	void UnsetBase( base::Base* base );

	void Render( size_t selected_unit_id = 0 );

	const std::unordered_map< size_t, unit::Unit* >& GetUnits() const;
	const std::vector< unit::Unit* >& GetOrderedUnits();
	unit::Unit* GetMostImportantUnit();

	Tile* W;
	Tile* NW;
	Tile* N;
	Tile* NE;
	Tile* E;
	Tile* SE;
	Tile* S;
	Tile* SW;
	Tile* GetNeighbour( const ::game::backend::map::tile::direction_t direction );

	struct render_data_t {
		types::Vec3 coords = {};
		::game::backend::map::tile::tile_vertices_t selection_coords = {};
		std::vector< types::mesh::Render* > preview_meshes = {};
		std::vector< std::string > preview_lines = {};
		std::vector< std::string > sprites = {};
	};

	const render_data_t& GetRenderData() const;

	void Update( const ::game::backend::map::tile::Tile& tile, const ::game::backend::map::tile::TileState& ts );

private:
	const types::Vec2< size_t > m_coords;

	struct {
		unit::Unit* currently_rendered_unit = nullptr;
		std::vector< unit::Unit* > currently_rendered_fake_badges = {};
	} m_render;
	std::unordered_map< size_t, unit::Unit* > m_units = {};
	std::vector< unit::Unit* > m_ordered_units = {};
	bool m_is_units_reorder_needed = true;

	bool m_is_water = false;

	base::Base* m_base = nullptr;

	render_data_t m_render_data = {};

};

}
}
}
