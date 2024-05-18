#pragma once

#include <string>

#include "types/Buffer.h"

namespace game {

namespace map {
namespace tile {
class Tile;
class TileState;
}
}

namespace unit {

class Render {
public:

	enum render_type_t {
		RT_SPRITE,
	};

	Render( const render_type_t type );
	virtual ~Render() = default;

	virtual const types::Vec3 GetSpawnCoords( const map::tile::Tile* tile, const map::tile::TileState* ts ) const = 0;
	virtual const std::string ToString( const std::string& prefix ) const = 0;

	const render_type_t m_type;

private:
	friend class StaticDef;

	static void Serialize( types::Buffer& buf, const Render* render );
	static Render* Unserialize( types::Buffer& buf );

};

}
}
