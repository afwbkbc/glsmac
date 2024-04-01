#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>

#include "types/Vec2.h"

#include "game/map/Tile.h"

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
	void Render( size_t selected_unit_id );
	const std::unordered_map< size_t, Unit* >& GetUnits() const;

	Tile* W;
	Tile* NW;
	Tile* N;
	Tile* NE;
	Tile* E;
	Tile* SE;
	Tile* S;
	Tile* SW;
	Tile* GetNeighbour( const ::game::map::Tile::direction_t direction );

private:
	const types::Vec2< size_t > m_coords;
	struct {
		Unit* currently_rendered_unit = nullptr;
		std::vector< Unit* > currently_rendered_fake_badges = {};
	} m_render;
	std::unordered_map< size_t, Unit* > m_units = {};

};

}
}

#include "Unit.h"
