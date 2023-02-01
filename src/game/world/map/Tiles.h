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
	
private:
	
	size_t m_width;
	size_t m_height;
	
	Tile::elevation_t* m_top_vertex_row;
	Tile::elevation_t* m_top_right_vertex_row;
	Tile* m_data;
	
	bool m_is_validated = false;
};
	
}
}
}
