#include "MapState.h"

#include <cmath>

#include "game/backend/settings/Types.h"

namespace game {
namespace backend {
namespace map {

MapState::~MapState() {
	//
}

tile::TileState* MapState::At( const size_t x, const size_t y ) {
	ASSERT( x < dimensions.x, "tile state x overflow" );
	ASSERT( y < dimensions.y, "tile state y overflow" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile state axis oddity differs" );
	return &m_tiles.at( y * ( dimensions.x / 2 ) + x / 2 );
}

const tile::TileState* MapState::AtConst( const size_t x, const size_t y ) const {
	ASSERT( x < dimensions.x, "tile state x overflow" );
	ASSERT( y < dimensions.y, "tile state y overflow" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile state axis oddity differs" );
	return &m_tiles.at( y * ( dimensions.x / 2 ) + x / 2 );
}

const std::vector< tile::TileState >* MapState::GetTileStatesPtr() const {
	return &m_tiles;
}

void MapState::LinkTileStates( MT_CANCELABLE ) {

	if ( !m_tiles.empty() ) {
		THROW( "tile states already linked" );
	}
	const uint64_t area = static_cast< uint64_t >( dimensions.x ) * dimensions.y;
	if (
		dimensions.x < settings::MAP_MIN_DIMENSION ||
		dimensions.y < settings::MAP_MIN_DIMENSION ||
		( dimensions.x & 1 ) ||
		( dimensions.y & 1 ) ||
		area > settings::MAP_MAX_AREA
	) {
		THROW( "invalid map-state dimensions" );
	}
	m_tiles.resize( static_cast< size_t >( area / 2 ) );

	Log( "Linking tile states" );

	// link to each other via pointers
	// TODO: refactor this and tile::Tiles
	for ( auto y = 0 ; y < dimensions.y ; y++ ) {
		for ( auto x = y & 1 ; x < dimensions.x ; x += 2 ) {
			auto* ts = At( x, y );

			ts->W = ( x >= 2 )
				? At( x - 2, y )
				: At( dimensions.x - 1 - ( 1 - ( y % 2 ) ), y );
			ts->NW = ( y >= 1 )
				? ( ( x >= 1 )
					? At( x - 1, y - 1 )
					: At( dimensions.x - 1, y - 1 )
				)
				: ts;
			ts->N = ( y >= 2 )
				? At( x, y - 2 )
				: ts;
			ts->NE = ( y >= 1 )
				? ( ( x < dimensions.x - 1 )
					? At( x + 1, y - 1 )
					: At( 0, y - 1 )
				)
				: ts;
			ts->E = ( x < dimensions.x - 2 )
				? At( x + 2, y )
				: At( y % 2, y );
			ts->SE = ( y < dimensions.y - 1 )
				? ( ( x < dimensions.x - 1 )
					? At( x + 1, y + 1 )
					: At( 0, y + 1 )
				)
				: ts;
			ts->S = ( y < dimensions.y - 2 )
				? At( x, y + 2 )
				: ts;
			ts->SW = ( y < dimensions.y - 1 )
				? ( ( x >= 1 )
					? At( x - 1, y + 1 )
					: At( dimensions.x - 1, y + 1 )
				)
				: ts;

			MT_RETIF();
		}
	}
}

const types::Buffer MapState::Serialize() const {
	types::Buffer buf;

	buf.WriteBool( first_run );
	buf.WriteVec2f( coord );
	buf.WriteVec2u( dimensions );

	buf.WriteVec2f( variables.texture_scaling );

	for ( auto y = 0 ; y < dimensions.y ; y++ ) {
		for ( auto x = y & 1 ; x < dimensions.x ; x += 2 ) {
			const auto* ts = AtConst( x, y );
			const auto b = ts->Serialize();
			const auto s = b.ToString();
			buf.WriteString( s );
		}
	}

	return buf;
}

void MapState::Deserialize( types::Buffer buf ) {

	const auto serialized_first_run = buf.ReadBool();
	const auto serialized_coord = buf.ReadVec2f();
	const auto serialized_dimensions = buf.ReadVec2u();
	const auto serialized_texture_scaling = buf.ReadVec2f();
	if (
		!std::isfinite( serialized_coord.x ) ||
		!std::isfinite( serialized_coord.y ) ||
		!std::isfinite( serialized_texture_scaling.x ) ||
		!std::isfinite( serialized_texture_scaling.y )
	) {
		THROW( "invalid serialized map-state coordinates" );
	}

	first_run = serialized_first_run;
	coord = serialized_coord;
	dimensions = serialized_dimensions;
	variables.texture_scaling = serialized_texture_scaling;

	MT_CANCELABLE = false;
	LinkTileStates( MT_C );

	for ( auto y = 0 ; y < dimensions.y ; y++ ) {
		for ( auto x = y & 1 ; x < dimensions.x ; x += 2 ) {
			At( x, y )->Deserialize( buf.ReadString() );
		}
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized map state" );
	}

	copy_from_after.clear();
}

}
}
}
