#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>

#include "game/map/tile/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"

namespace types::mesh {
class Render;
}

namespace game::map::tile {
class Tile;
class TileState;
}

namespace task {
namespace game {

class Unit;

class Tile {
public:

	static std::vector< size_t > GetUnitsOrder( const std::unordered_map< size_t, Unit* >& units );

	Tile( const types::Vec2< size_t >& coords );

	const types::Vec2< size_t >& GetCoords() const;

	void AddUnit( Unit* unit );
	void RemoveUnit( Unit* unit );
	void SetActiveUnit( Unit* unit );

	void Render( size_t selected_unit_id = 0 );

	const std::unordered_map< size_t, Unit* >& GetUnits() const;
	const std::vector< Unit* >& GetOrderedUnits();
	Unit* GetMostImportantUnit();

	Tile* W;
	Tile* NW;
	Tile* N;
	Tile* NE;
	Tile* E;
	Tile* SE;
	Tile* S;
	Tile* SW;
	Tile* GetNeighbour( const ::game::map::tile::direction_t direction );

	struct render_data_t {
		types::Vec3 coords = {};
		::game::map::tile::tile_vertices_t selection_coords = {};
		std::vector< types::mesh::Render* > preview_meshes = {};
		std::vector< std::string > preview_lines = {};
		std::vector< std::string > sprites = {};
	};

	const render_data_t& GetRenderData() const;

	void SetCoords( const types::Vec3& coords );
	void SetSelectionCoords( const ::game::map::tile::tile_vertices_t& selection_coords );
	void SetPreviewMeshes( const std::vector< types::mesh::Render* >& preview_meshes );
	void SetPreviewLines( const std::vector< std::string >& preview_lines );
	void SetSprites( const std::vector< std::string >& sprites );

private:
	const types::Vec2< size_t > m_coords;
	struct {
		Unit* currently_rendered_unit = nullptr;
		std::vector< Unit* > currently_rendered_fake_badges = {};
	} m_render;
	std::unordered_map< size_t, Unit* > m_units = {};
	std::vector< Unit* > m_ordered_units = {};
	bool m_is_units_reorder_needed = true;
	render_data_t m_render_data = {};
};

}
}
