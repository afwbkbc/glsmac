#pragma once

#include "base/Base.h"

#include "Tile.h"

namespace game {
namespace world {
namespace map {

CLASS( Tiles, base::Base )
	
	Tiles( const size_t width, const size_t height );
	~Tiles();
	
	const size_t GetWidth() const;
	const size_t GetHeight() const;
	
	Tile* At( const size_t x, const size_t y ) const;
	Tile::elevation_t* TopVertexAt( const size_t x, const size_t y ) const;
	Tile::elevation_t* TopRightVertexAt( const size_t x ) const;

	void Validate();
	void Finalize();
	
	// you can call it from map generator when you think you may have generated extreme slopes
	// if you don't and keep generating - they will be normalized more aggressively at the end and may make terrain more flat
	void FixExtremeSlopes();
	
private:
	
	size_t m_width;
	size_t m_height;
	
	Tile::elevation_t* m_top_vertex_row;
	Tile::elevation_t* m_top_right_vertex_row;
	Tile* m_data;
	
	bool m_is_validated = false;
	
	void NormalizeElevationRange();
	void SetLandAmount( const float amount ); // 0.0f - 1.0f. can be slow
	const float GetLandAmount( Tile::elevation_t elevation_diff = 0.0f ); // determine how much land there would be with specified elevation difference
	void RaiseAllTilesBy( Tile::elevation_t amount );
	const pair< Tile::elevation_t, Tile::elevation_t > GetElevationsRange() const;
	void RemoveExtremeSlopes( const Tile::elevation_t max_allowed_diff );
	void FixTopBottomRows();
};
	
}
}
}
