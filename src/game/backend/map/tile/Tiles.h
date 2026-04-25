#pragma once

#include <vector>

#include "types/Serializable.h"

#include "Tile.h"
#include "common/MTTypes.h"

namespace util::random {
class Random;
}

namespace game {
namespace backend {
namespace map {
namespace tile {

CLASS( Tiles, types::Serializable )

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
	std::vector< Tile >* GetTilesPtr();
	elevation_t* TopVertexAt( const size_t x, const size_t y );
	elevation_t* TopRightVertexAt( const size_t x );

	void Validate( MT_CANCELABLE );

	const size_t GetDataCount() const;

	void FixTopBottomRows( util::random::Random* random );

	const std::vector< Tile* > GetVector( MT_CANCELABLE );

	const types::Buffer Serialize() const override;
	void Deserialize( types::Buffer buf ) override;

private:
	uint32_t m_width = 0;
	uint32_t m_height = 0;

	std::vector< elevation_t > m_top_vertex_row = {};
	std::vector< elevation_t > m_top_right_vertex_row = {};
	std::vector< Tile > m_data = {};

	bool m_is_validated = false;

};

}
}
}
}
