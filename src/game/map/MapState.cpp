#include "MapState.h"

namespace game {
namespace map {

MapState::~MapState() {
	//
}

TileState& MapState::At( const size_t x, const size_t y ) {
	ASSERT( x < dimensions.x, "tile state x overflow" );
	ASSERT( y < dimensions.y, "tile state y overflow" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile state axis oddity differs" );
	return m_tile_states.at( y * dimensions.x + x / 2 );
}

const TileState& MapState::AtConst( const size_t x, const size_t y ) const {
	ASSERT( x < dimensions.x, "tile state x overflow" );
	ASSERT( y < dimensions.y, "tile state y overflow" );
	ASSERT( ( x % 2 ) == ( y % 2 ), "tile state axis oddity differs" );
	return m_tile_states.at( y * dimensions.x + x / 2 );
}

void MapState::LinkTileStates( MT_CANCELABLE ) {

	ASSERT( m_tile_states.empty(), "m_tile_states already set" );
	m_tile_states.resize( dimensions.y * dimensions.x );

	Log( "Linking tile states" );

	// link to each other via pointers
	// TODO: refactor this and Tiles
	for ( auto y = 0 ; y < dimensions.y ; y++ ) {
		for ( auto x = y & 1 ; x < dimensions.x ; x += 2 ) {
			auto* ts = &At( x, y );

			ts->W = ( x >= 2 )
				? &At( x - 2, y )
				: &At( dimensions.x - 1 - ( 1 - ( y % 2 ) ), y );
			ts->NW = ( y >= 1 )
				? ( ( x >= 1 )
					? &At( x - 1, y - 1 )
					: &At( dimensions.x - 1, y - 1 )
				)
				: ts;
			ts->N = ( y >= 2 )
				? &At( x, y - 2 )
				: ts;
			ts->NE = ( y >= 1 )
				? ( ( x < dimensions.x - 1 )
					? &At( x + 1, y - 1 )
					: &At( 0, y - 1 )
				)
				: ts;
			ts->E = ( x < dimensions.x - 2 )
				? &At( x + 2, y )
				: &At( y % 2, y );
			ts->SE = ( y < dimensions.y - 1 )
				? ( ( x < dimensions.x - 1 )
					? &At( x + 1, y + 1 )
					: &At( 0, y + 1 )
				)
				: ts;
			ts->S = ( y < dimensions.y - 2 )
				? &At( x, y + 2 )
				: ts;
			ts->SW = ( y < dimensions.y - 1 )
				? ( ( x >= 1 )
					? &At( x - 1, y + 1 )
					: &At( dimensions.x - 1, y + 1 )
				)
				: ts;

			MT_RETIF();
		}
	}
}

const Buffer MapState::Serialize() const {
	Buffer buf;

	buf.WriteBool( first_run );
	buf.WriteVec2f( coord );
	buf.WriteVec2u( dimensions );

	buf.WriteVec2f( variables.texture_scaling );

	for ( auto y = 0 ; y < dimensions.y ; y++ ) {
		for ( auto x = y & 1 ; x < dimensions.x ; x += 2 ) {
			const auto* ts = &AtConst( x, y );
			const auto b = ts->Serialize();
			const auto s = b.ToString();
			buf.WriteString( s );
		}
	}

	return buf;
}

void MapState::Unserialize( Buffer buf ) {

	first_run = buf.ReadBool();
	coord = buf.ReadVec2f();
	dimensions = buf.ReadVec2u();

	variables.texture_scaling = buf.ReadVec2f();

	MT_CANCELABLE = false;
	LinkTileStates( MT_C );

	for ( auto y = 0 ; y < dimensions.y ; y++ ) {
		for ( auto x = y & 1 ; x < dimensions.x ; x += 2 ) {
			At( x, y ).Unserialize( buf.ReadString() );
		}
	}

	copy_from_after.clear();
}

}
}
