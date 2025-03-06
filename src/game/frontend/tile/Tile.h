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

class TileObject;

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
	~Tile();

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
	const std::vector< TileObject* >& GetOrderedObjects();
	unit::Unit* GetMostImportantUnit();
	TileObject* GetMostImportantObject();

	base::Base* GetBase() const;

	Tile* W;
	Tile* NW;
	Tile* N;
	Tile* NE;
	Tile* E;
	Tile* SE;
	Tile* S;
	Tile* SW;
	Tile* GetNeighbour( const backend::map::tile::direction_t direction );

	const types::Vec2< size_t > m_coords;

	struct render_data_t {
		types::Vec3 coords = {};
		backend::map::tile::tile_vertices_t selection_coords = {};
		std::vector< types::mesh::Render* > preview_meshes = {};
		std::vector< std::string > preview_lines = {};
		std::vector< std::string > sprites = {};
	};

	const render_data_t& GetRenderData() const;
	const backend::map::tile::yields_t& GetYields() const;

	void Update( const backend::map::tile::Tile& tile, const backend::map::tile::TileState& ts );

private:

	struct {
		unit::Unit* currently_rendered_unit = nullptr;
		std::vector< unit::Unit* > currently_rendered_fake_badges = {};
	} m_render;
	std::unordered_map< size_t, unit::Unit* > m_units = {};

	std::vector< unit::Unit* > m_ordered_units = {};
	bool m_is_units_reorder_needed = true;

	std::vector< TileObject* > m_ordered_objects = {};
	bool m_is_objects_reorder_needed = true;

	bool m_is_water = false;

	base::Base* m_base = nullptr;

	render_data_t m_render_data = {};

	backend::map::tile::yields_t m_yields = {};

};

}
}
}
