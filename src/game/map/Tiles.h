#pragma once

#include <vector>

#include "types/Serializable.h"

#include "Tile.h"
#include "util/Random.h"
#include "base/MTModule.h"

using namespace types;
using namespace util;

namespace game {
namespace map {

CLASS( Tiles, Serializable )

	Tiles( const uint32_t width = 0, const uint32_t height = 0 );
	~Tiles();

	// warning: this will reset all tiles
	void Resize( const uint32_t width, const uint32_t height );

	// reset to empty state
	void Clear();

	const uint32_t GetWidth() const;
	const uint32_t GetHeight() const;

	Tile& At( const size_t x, const size_t y );
	const Tile& AtConst( const size_t x, const size_t y ) const;
	Tile::elevation_t* TopVertexAt( const size_t x, const size_t y );
	Tile::elevation_t* TopRightVertexAt( const size_t x );

	void Validate( MT_CANCELABLE );

	const size_t GetDataCount() const;

	void FixTopBottomRows( Random* random );

	const std::vector< Tile* > GetVector( MT_CANCELABLE );

	const Buffer Serialize() const override;
	void Unserialize( Buffer buf ) override;

private:
	uint32_t m_width = 0;
	uint32_t m_height = 0;

	std::vector< Tile::elevation_t > m_top_vertex_row = {};
	std::vector< Tile::elevation_t > m_top_right_vertex_row = {};
	std::vector< Tile > m_data = {};

	bool m_is_validated = false;

};

}
}
