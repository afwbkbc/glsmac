#pragma once

#include <string>

#include "types/Buffer.h"

#include "game/map/TileState.h"

#include "gse/Value.h"

namespace game {
namespace unit {

class Def {
public:

	enum def_type_t {
		DT_STATIC,
	};

	Def( const std::string& id, const def_type_t type, const std::string& name );
	virtual ~Def() = default;

	const std::string m_id;
	const def_type_t m_type;
	const std::string m_name;

	virtual const types::Vec3 GetSpawnCoords( const float tile_x, const float tile_y, const map::TileState::tile_vertices_t& tile_coords ) const = 0;

	static const types::Buffer Serialize( const Def* def );
	static Def* Unserialize( types::Buffer& buf );

	WRAPDEFS_PTR( Def );
};

}
}
