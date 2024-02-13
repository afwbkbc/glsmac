#pragma once

#include "types/Buffer.h"

#include "game/map/TileState.h"

namespace game {
namespace unit {

class Render {
public:

	enum render_type_t {
		RT_SPRITE,
	};

	Render( const render_type_t type );

	virtual const types::Vec3 GetSpawnCoords( const float tile_x, const float tile_y, const map::TileState::tile_vertices_t& tile_coords ) const = 0;

	const render_type_t m_type;

private:
	friend class StaticDef;

	static void Serialize( types::Buffer& buf, const Render* render );
	static Render* Unserialize( types::Buffer& buf );

};

}
}
