#pragma once

#include "base/Base.h"

#include "TileState.h"
#include "base/MTModule.h"

namespace game {
namespace map {
	
CLASS( MapState, base::Base )
	
	~MapState();

	struct copy_from_after_t {
		Texture::add_flag_t mode;
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
	Vec2< float > coord;
	Vec2< uint32_t > dimensions;
	struct {
		Vec2< float > texture_scaling;
	} variables;
	std::vector< copy_from_after_t > copy_from_after;

	const Texture* terrain_texture;
	const Texture* ter1_pcx;
	
	TileState* At( const size_t x, const size_t y ) const;

	void LinkTileStates( MT_CANCELABLE );
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
	
	TileState* GetTileState( const size_t x, const size_t y ) const;
	
private:
	TileState* m_tile_states = nullptr;
	
};

}
}
