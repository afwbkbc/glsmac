#include "TileState.h"

namespace game {
namespace map {

const Buffer TileState::Serialize() const {
	Buffer buf;
	
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
	buf.WriteString( overdraw_column.coords.Serialize().ToString() );
	buf.WriteString( overdraw_column.indices.Serialize().ToString() );
	buf.WriteString( overdraw_column.surfaces.Serialize().ToString() );
	buf.WriteString( data_mesh.coords.Serialize().ToString() );
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

const Buffer TileState::tile_elevations_t::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( center );
	buf.WriteInt( left );
	buf.WriteInt( top );
	buf.WriteInt( right );
	buf.WriteInt( bottom );
	
	return buf;
}

const Buffer TileState::tile_layer_t::Serialize() const {
	Buffer buf;
	
	buf.WriteString( coords.Serialize().ToString() );
	buf.WriteString( indices.Serialize().ToString() );
	buf.WriteString( surfaces.Serialize().ToString() );
	buf.WriteString( tex_coords.Serialize().ToString() );
	buf.WriteString( colors.Serialize().ToString() );
	buf.WriteVec2f( texture_stretch );
	buf.WriteBool( texture_stretch_at_edges );
	
	return buf;
}

const Buffer TileState::tile_vertices_t::Serialize() const {
	Buffer buf;
	
	buf.WriteVec3( center );
	buf.WriteVec3( left );
	buf.WriteVec3( top );
	buf.WriteVec3( right );
	buf.WriteVec3( bottom );
	
	return buf;
}

const Buffer TileState::tile_indices_t::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( center );
	buf.WriteInt( left );
	buf.WriteInt( top );
	buf.WriteInt( right );
	buf.WriteInt( bottom );
	
	return buf;
}

const Buffer TileState::tile_surfaces_t::Serialize() const {
	Buffer buf;
	
	buf.WriteInt( left_top );
	buf.WriteInt( top_right );
	buf.WriteInt( right_bottom );
	buf.WriteInt( bottom_left );
	
	return buf;
}

const Buffer TileState::tile_tex_coords_t::Serialize() const {
	Buffer buf;
	
	buf.WriteVec2f( center );
	buf.WriteVec2f( left );
	buf.WriteVec2f( top );
	buf.WriteVec2f( right );
	buf.WriteVec2f( bottom );
	
	return buf;
}

const Buffer TileState::tile_colors_t::Serialize() const {
	Buffer buf;
	
	buf.WriteColor( center );
	buf.WriteColor( left );
	buf.WriteColor( top );
	buf.WriteColor( right );
	buf.WriteColor( bottom );
	
	return buf;
}

void TileState::Unserialize( Buffer buf ) {
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
		layers[i].Unserialize( buf.ReadString() );
	}
	overdraw_column.coords.Unserialize( buf.ReadString() );
	overdraw_column.indices.Unserialize( buf.ReadString() );
	overdraw_column.surfaces.Unserialize( buf.ReadString() );
	data_mesh.coords.Unserialize( buf.ReadString() );
	data_mesh.indices.Unserialize( buf.ReadString() );
	has_water = buf.ReadBool();
	is_coastline_corner = buf.ReadBool();
	
	const auto w = s_consts.tc.texture_pcx.dimensions.x;
	const auto h = s_consts.tc.texture_pcx.dimensions.y;
	
	NEW( moisture_original, Texture, "MoistureOriginal", w, h );
	moisture_original->Unserialize( buf.ReadString() );
	const bool has_river_original = buf.ReadBool();
	if ( has_river_original ) {
		NEW( river_original, Texture, "RiverOriginal", w, h );
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

void TileState::tile_elevations_t::Unserialize( Buffer buf ) {
	center = buf.ReadInt();
	left = buf.ReadInt();
	top = buf.ReadInt();
	right = buf.ReadInt();
	bottom = buf.ReadInt();
}

void TileState::tile_layer_t::Unserialize( Buffer buf ) {
	coords.Unserialize( buf.ReadString() );
	indices.Unserialize( buf.ReadString() );
	surfaces.Unserialize( buf.ReadString() );
	tex_coords.Unserialize( buf.ReadString() );
	colors.Unserialize( buf.ReadString() );
	texture_stretch = buf.ReadVec2f();
	texture_stretch_at_edges = buf.ReadBool();
}

void TileState::tile_vertices_t::Unserialize( Buffer buf ) {
	center = buf.ReadVec3();
	left = buf.ReadVec3();
	top = buf.ReadVec3();
	right = buf.ReadVec3();
	bottom = buf.ReadVec3();
}

void TileState::tile_indices_t::Unserialize( Buffer buf ) {
	center = buf.ReadInt();
	left = buf.ReadInt();
	top = buf.ReadInt();
	right = buf.ReadInt();
	bottom = buf.ReadInt();
}

void TileState::tile_surfaces_t::Unserialize( Buffer buf ) {
	left_top = buf.ReadInt();
	top_right = buf.ReadInt();
	right_bottom = buf.ReadInt();
	bottom_left = buf.ReadInt();
}

void TileState::tile_tex_coords_t::Unserialize( Buffer buf ) {
	center = buf.ReadVec2f();
	left = buf.ReadVec2f();
	top = buf.ReadVec2f();
	right = buf.ReadVec2f();
	bottom = buf.ReadVec2f();
}

void TileState::tile_colors_t::Unserialize( Buffer buf ) {
	center = buf.ReadColor();
	left = buf.ReadColor();
	top = buf.ReadColor();
	right = buf.ReadColor();
	bottom = buf.ReadColor();
}

}
}
