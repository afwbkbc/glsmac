#pragma once

#include "types/Serializable.h"

#include "Tile.h"

#include "util/Random.h"

using namespace types;
using namespace util;

namespace game {
namespace world {
namespace map {

CLASS( Tiles, Serializable )
	
	Tiles( const uint32_t width, const uint32_t height, Random* random );
	~Tiles();
	
	// warning: will reset all tiles
	void Resize( const uint32_t width, const uint32_t height );
	
	const uint32_t GetWidth() const;
	const uint32_t GetHeight() const;
	
	Tile* At( const size_t x, const size_t y ) const;
	Tile::elevation_t* TopVertexAt( const size_t x, const size_t y ) const;
	Tile::elevation_t* TopRightVertexAt( const size_t x ) const;

	void Validate();
	void Finalize();
	
	// you can call it from map generator when you think you may have generated extreme slopes
	// if you don't and keep generating - they will be normalized more aggressively at the end and may make terrain more flat
	void FixExtremeSlopes();
	
	Random* GetRandom() const;
	
	const size_t GetDataCount() const;
	
	// be very careful with this
	const Tile* GetDataPtr() const;
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
	
private:
	
	Random* m_random = nullptr;
	
	uint32_t m_width;
	uint32_t m_height;
	
	Tile::elevation_t* m_top_vertex_row = nullptr;
	Tile::elevation_t* m_top_right_vertex_row = nullptr;
	size_t m_data_count = 0;
	Tile* m_data = nullptr;
	
	bool m_is_validated = false;
	
	void NormalizeElevationRange();
	void SetLandAmount( const float amount ); // 0.0f - 1.0f. can be slow
	const float GetLandAmount( Tile::elevation_t elevation_diff = 0.0f ); // determine how much land there would be with specified elevation difference
	void RaiseAllTilesBy( Tile::elevation_t amount );
	const std::pair< Tile::elevation_t, Tile::elevation_t > GetElevationsRange() const;
	void RemoveExtremeSlopes( const Tile::elevation_t max_allowed_diff );
	void FixTopBottomRows();
};
	
}
}
}
