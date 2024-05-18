#pragma once

#include <vector>

#include "base/Base.h"

#include "types/texture/Types.h"

#include "base/MTTypes.h"
#include "types/Buffer.h"
#include "types/Vec2.h"
#include "tile/TileState.h"

#include "base/MTModule.h"

namespace types {
class Texture;
}

namespace util {
class Perlin;
}

namespace game {
namespace map {

CLASS( MapState, base::Base )

	~MapState();

	struct copy_from_after_t {
		types::texture::add_flag_t mode;
		size_t tx1_from;
		size_t ty1_from;
		size_t tx2_from;
		size_t ty2_from;
		size_t tx_to;
		size_t ty_to;
		uint8_t rotate;
		float alpha;
		util::Perlin* perlin = nullptr;
	};

	bool first_run;
	types::Vec2< float > coord;
	types::Vec2< uint32_t > dimensions;
	struct {
		types::Vec2< float > texture_scaling;
	} variables;
	std::vector< copy_from_after_t > copy_from_after;

	const types::texture::Texture* terrain_texture;
	const types::texture::Texture* ter1_pcx;

	tile::TileState* At( const size_t x, const size_t y );
	const tile::TileState* AtConst( const size_t x, const size_t y ) const;
	const std::vector< tile::TileState >* GetTileStatesPtr() const;

	void LinkTileStates( MT_CANCELABLE );

	const types::Buffer Serialize() const;
	void Unserialize( types::Buffer buf );

private:
	std::vector< tile::TileState > m_tiles = {};

};

}
}
