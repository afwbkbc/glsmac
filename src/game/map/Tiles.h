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

	Tile* At( const size_t x, const size_t y ) const;
	Tile::elevation_t* TopVertexAt( const size_t x, const size_t y ) const;
	Tile::elevation_t* TopRightVertexAt( const size_t x ) const;

	void Validate( MT_CANCELABLE );

	// to be called after map generation. it will normalize heights, remove extreme slopes and fix other things
	// it will also try to match generated tiles to desired map settings such as land amount, moisture, elevations, fungus
	// return true if all went fine, false if it was unable to get good state and need to regenerate everything again
	// in map generators you MUST call this in the end, providing reference to seed
	// if it returns false - it will change seed and you'll need to regenerate everything from new seed
	// generate elevation-dependent changes AFTER Finalize(), because Finalize will change elevations
	// common usage in map generators:
	//     do { ... generate elevations ... } while ( !tiles->Finalize( &seed ) ); ... generate other things ...
	// 
	bool Finalize( size_t* seed );

	const size_t GetDataCount() const;

	// be very careful with this
	const Tile* GetDataPtr() const;

	void FixTopBottomRows( Random* random );

	const std::vector< Tile* > GetVector( MT_CANCELABLE ) const;

	const Buffer Serialize() const override;
	void Unserialize( Buffer buf ) override;

private:
	uint32_t m_width = 0;
	uint32_t m_height = 0;

	Tile::elevation_t* m_top_vertex_row = nullptr;
	Tile::elevation_t* m_top_right_vertex_row = nullptr;
	size_t m_data_count = 0;
	Tile* m_data = nullptr;

	bool m_is_validated = false;

	void SetLandAmount( const float amount ); // 0.0f - 1.0f. can be slow
	const float GetLandAmount( Tile::elevation_t elevation_diff = 0.0f ); // determine how much land there would be with specified elevation difference
	void RaiseAllTilesBy( Tile::elevation_t amount );
	const std::pair< Tile::elevation_t, Tile::elevation_t > GetElevationsRange() const;
};

}
}
