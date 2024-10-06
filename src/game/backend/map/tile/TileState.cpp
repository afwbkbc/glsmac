#include "TileState.h"

#include "game/backend/map/Consts.h"
#include "types/texture/Texture.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

TileState::~TileState() {
	if ( moisture_original ) {
		DELETE( moisture_original );
	}
	if ( river_original ) {
		DELETE( river_original );
	}
/*	for ( auto& a : ts->sprites ) {
		a.actor->RemoveInstance( a.instance );
	}*/
}

TileState* TileState::GetNeighbour( const direction_t direction ) {
	switch ( direction ) {
		case D_NONE:
			return this;
		case D_W:
			return W;
		case D_NW:
			return NW;
		case D_N:
			return N;
		case D_NE:
			return NE;
		case D_E:
			return E;
		case D_SE:
			return SE;
		case D_S:
			return S;
		case D_SW:
			return SW;
		default:
			THROW( "unknown tile direction: " + std::to_string( direction ) );
	}
}

const types::Vec3& TileState::GetCenterCoords( tile_layer_type_t layer ) const {
	ASSERT_NOLOG( layer <= LAYER_MAX, "layer overflow" );
	return layers[ layer ].coords.center;
}

const types::Buffer TileState::Serialize() const {
	types::Buffer buf;

	buf.WriteVec2f( coord );
	buf.WriteFloat( tex_coord.x );
	buf.WriteFloat( tex_coord.y );
	buf.WriteFloat( tex_coord.x1 );
	buf.WriteFloat( tex_coord.y1 );
	buf.WriteFloat( tex_coord.x2 );
	buf.WriteFloat( tex_coord.y2 );
	buf.WriteString( elevations.Serialize().ToString() );
	buf.WriteInt( LAYER_MAX );
	for ( auto i = 0 ; i < LAYER_MAX ; i++ ) {
		buf.WriteString( layers[ i ].Serialize().ToString() );
	}
	buf.WriteString( SerializeTileVertices( overdraw_column.coords ).ToString() );
	buf.WriteString( overdraw_column.indices.Serialize().ToString() );
	buf.WriteString( overdraw_column.surfaces.Serialize().ToString() );
	buf.WriteString( SerializeTileVertices( data_mesh.coords ).ToString() );
	buf.WriteString( data_mesh.indices.Serialize().ToString() );
	buf.WriteBool( has_water );
	buf.WriteBool( is_coastline_corner );
	buf.WriteString( moisture_original->Serialize().ToString() );
	if ( river_original ) {
		buf.WriteBool( true );
		buf.WriteString( river_original->Serialize().ToString() );
	}
	else {
		buf.WriteBool( false );
	}

	buf.WriteInt( sprites.size() );
	for ( auto& a : sprites ) {
		buf.WriteString( a.actor );
		buf.WriteInt( a.instance );
		buf.WriteString( a.name );
		buf.WriteVec2u( a.tex_coords );
	}

	return buf;
}

const types::Buffer TileState::tile_elevations_t::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( center );
	buf.WriteInt( left );
	buf.WriteInt( top );
	buf.WriteInt( right );
	buf.WriteInt( bottom );

	return buf;
}

const types::Buffer TileState::tile_layer_t::Serialize() const {
	types::Buffer buf;

	buf.WriteString( SerializeTileVertices( coords ).ToString() );
	buf.WriteString( indices.Serialize().ToString() );
	buf.WriteString( surfaces.Serialize().ToString() );
	buf.WriteString( SerializeTileTexCoords( tex_coords ).ToString() );
	buf.WriteString( SerializeTileColors( colors ).ToString() );
	buf.WriteVec2f( texture_stretch );
	buf.WriteBool( texture_stretch_at_edges );

	return buf;
}

const types::Buffer TileState::tile_indices_t::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( center );
	buf.WriteInt( left );
	buf.WriteInt( top );
	buf.WriteInt( right );
	buf.WriteInt( bottom );

	return buf;
}

const types::Buffer TileState::tile_surfaces_t::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( left_top );
	buf.WriteInt( top_right );
	buf.WriteInt( right_bottom );
	buf.WriteInt( bottom_left );

	return buf;
}

void TileState::Unserialize( types::Buffer buf ) {
	coord = buf.ReadVec2f();
	tex_coord.x = buf.ReadFloat();
	tex_coord.y = buf.ReadFloat();
	tex_coord.x1 = buf.ReadFloat();
	tex_coord.y1 = buf.ReadFloat();
	tex_coord.x2 = buf.ReadFloat();
	tex_coord.y2 = buf.ReadFloat();
	elevations.Unserialize( buf.ReadString() );
	if ( (tile_layer_type_t)buf.ReadInt() != LAYER_MAX ) {
		THROW( "LAYER_MAX mismatch" );
	}
	for ( auto i = 0 ; i < LAYER_MAX ; i++ ) {
		layers[ i ].Unserialize( buf.ReadString() );
	}
	overdraw_column.coords = UnserializeTileVertices( buf.ReadString() );
	overdraw_column.indices.Unserialize( buf.ReadString() );
	overdraw_column.surfaces.Unserialize( buf.ReadString() );
	data_mesh.coords = UnserializeTileVertices( buf.ReadString() );
	data_mesh.indices.Unserialize( buf.ReadString() );
	has_water = buf.ReadBool();
	is_coastline_corner = buf.ReadBool();

	const auto w = s_consts.tc.texture_pcx.dimensions.x;
	const auto h = s_consts.tc.texture_pcx.dimensions.y;

	NEW( moisture_original, types::texture::Texture, "MoistureOriginal", w, h );
	moisture_original->Unserialize( buf.ReadString() );
	const bool has_river_original = buf.ReadBool();
	if ( has_river_original ) {
		NEW( river_original, types::texture::Texture, "RiverOriginal", w, h );
		river_original->Unserialize( buf.ReadString() );
	}

	const size_t sprites_count = buf.ReadInt();
	sprites.clear();
	for ( size_t i = 0 ; i < sprites_count ; i++ ) {
		sprite_t sprite;
		sprite.actor = buf.ReadString();
		sprite.instance = buf.ReadInt();
		sprite.name = buf.ReadString();
		sprite.tex_coords = buf.ReadVec2u();
		sprites.push_back( sprite );
	}

}

const types::Buffer TileState::SerializeTileVertices( const tile_vertices_t& vertices ) {
	types::Buffer buf;

	buf.WriteVec3( vertices.center );
	buf.WriteVec3( vertices.left );
	buf.WriteVec3( vertices.top );
	buf.WriteVec3( vertices.right );
	buf.WriteVec3( vertices.bottom );

	return buf;
}

const tile_vertices_t TileState::UnserializeTileVertices( types::Buffer buf ) {
	const auto center = buf.ReadVec3();
	const auto left = buf.ReadVec3();
	const auto top = buf.ReadVec3();
	const auto right = buf.ReadVec3();
	const auto bottom = buf.ReadVec3();
	return {
		center,
		left,
		top,
		right,
		bottom
	};
}

const types::Buffer TileState::SerializeTileTexCoords( const tile_tex_coords_t& tex_coords ) {
	types::Buffer buf;

	buf.WriteVec2f( tex_coords.center );
	buf.WriteVec2f( tex_coords.left );
	buf.WriteVec2f( tex_coords.top );
	buf.WriteVec2f( tex_coords.right );
	buf.WriteVec2f( tex_coords.bottom );

	return buf;
}

const tile_tex_coords_t TileState::UnserializeTileTexCoords( types::Buffer buf ) {
	const auto center = buf.ReadVec2f();
	const auto left = buf.ReadVec2f();
	const auto top = buf.ReadVec2f();
	const auto right = buf.ReadVec2f();
	const auto bottom = buf.ReadVec2f();
	return {
		center,
		left,
		top,
		right,
		bottom
	};
}

const types::Buffer TileState::SerializeTileColors( const tile_colors_t& colors ) {
	types::Buffer buf;

	buf.WriteColor( colors.center );
	buf.WriteColor( colors.left );
	buf.WriteColor( colors.top );
	buf.WriteColor( colors.right );
	buf.WriteColor( colors.bottom );

	return buf;
}

const tile_colors_t TileState::UnserializeTileColors( types::Buffer buf ) {
	const auto center = buf.ReadColor();
	const auto left = buf.ReadColor();
	const auto top = buf.ReadColor();
	const auto right = buf.ReadColor();
	const auto bottom = buf.ReadColor();
	return {
		center,
		left,
		top,
		right,
		bottom
	};
}

void TileState::tile_elevations_t::Unserialize( types::Buffer buf ) {
	center = buf.ReadInt();
	left = buf.ReadInt();
	top = buf.ReadInt();
	right = buf.ReadInt();
	bottom = buf.ReadInt();
}

void TileState::tile_layer_t::Unserialize( types::Buffer buf ) {
	coords = UnserializeTileVertices( buf.ReadString() );
	indices.Unserialize( buf.ReadString() );
	surfaces.Unserialize( buf.ReadString() );
	tex_coords = UnserializeTileTexCoords( buf.ReadString() );
	colors = UnserializeTileColors( buf.ReadString() );
	texture_stretch = buf.ReadVec2f();
	texture_stretch_at_edges = buf.ReadBool();
}

void TileState::tile_indices_t::Unserialize( types::Buffer buf ) {
	center = buf.ReadInt();
	left = buf.ReadInt();
	top = buf.ReadInt();
	right = buf.ReadInt();
	bottom = buf.ReadInt();
}

void TileState::tile_surfaces_t::Unserialize( types::Buffer buf ) {
	left_top = buf.ReadInt();
	top_right = buf.ReadInt();
	right_bottom = buf.ReadInt();
	bottom_left = buf.ReadInt();
}

}
}
}
}
