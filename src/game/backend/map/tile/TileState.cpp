#include "TileState.h"

#include <cmath>
#include <memory>
#include <utility>

#include "game/backend/map/Consts.h"
#include "types/texture/Texture.h"

namespace game {
namespace backend {
namespace map {
namespace tile {

static bool IsFinite( const types::Vec2< float >& value ) {
	return std::isfinite( value.x ) && std::isfinite( value.y );
}

static bool IsFinite( const types::Vec3& value ) {
	return std::isfinite( value.x ) && std::isfinite( value.y ) && std::isfinite( value.z );
}

static bool IsFinite( const types::Color& value ) {
	return
		std::isfinite( value.value.red ) &&
		std::isfinite( value.value.green ) &&
		std::isfinite( value.value.blue ) &&
		std::isfinite( value.value.alpha );
}

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
	if ( layer < 0 || layer >= LAYER_MAX ) {
		THROW( "tile-state layer overflow" );
	}
	return layers[ layer ].coords.center;
}

void TileState::ValidateMeshReferences(
	const size_t terrain_vertex_count,
	const size_t terrain_surface_count,
	const size_t data_vertex_count
) const {
	const auto validate_indices = []( const tile_indices_t& indices, const size_t count ) {
		if (
			indices.center >= count ||
			indices.left >= count ||
			indices.top >= count ||
			indices.right >= count ||
			indices.bottom >= count
		) {
			THROW( "serialized tile state references an out-of-bounds mesh vertex" );
		}
	};
	const auto validate_surfaces = []( const tile_surfaces_t& surfaces, const size_t count ) {
		if (
			surfaces.left_top >= count ||
			surfaces.top_right >= count ||
			surfaces.right_bottom >= count ||
			surfaces.bottom_left >= count
		) {
			THROW( "serialized tile state references an out-of-bounds mesh surface" );
		}
	};

	for ( const auto& layer : layers ) {
		validate_indices( layer.indices, terrain_vertex_count );
		validate_surfaces( layer.surfaces, terrain_surface_count );
	}
	validate_indices( overdraw_column.indices, terrain_vertex_count );
	validate_surfaces( overdraw_column.surfaces, terrain_surface_count );
	validate_indices( data_mesh.indices, data_vertex_count );
}

const types::Buffer TileState::Serialize() const {
	if ( !moisture_original ) {
		THROW( "cannot serialize tile state without its moisture texture" );
	}
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

void TileState::Deserialize( types::Buffer buf ) {
	const auto serialized_coord = buf.ReadVec2f();
	const auto tex_x = buf.ReadFloat();
	const auto tex_y = buf.ReadFloat();
	const auto tex_x1 = buf.ReadFloat();
	const auto tex_y1 = buf.ReadFloat();
	const auto tex_x2 = buf.ReadFloat();
	const auto tex_y2 = buf.ReadFloat();
	if (
		!IsFinite( serialized_coord ) ||
		!std::isfinite( tex_x ) ||
		!std::isfinite( tex_y ) ||
		!std::isfinite( tex_x1 ) ||
		!std::isfinite( tex_y1 ) ||
		!std::isfinite( tex_x2 ) ||
		!std::isfinite( tex_y2 )
	) {
		THROW( "serialized tile state contains non-finite texture coordinates" );
	}

	tile_elevations_t serialized_elevations = {};
	serialized_elevations.Deserialize( buf.ReadString() );
	if ( buf.ReadInt() != LAYER_MAX ) {
		THROW( "LAYER_MAX mismatch" );
	}
	tile_layer_t serialized_layers[LAYER_MAX] = {};
	for ( auto i = 0 ; i < LAYER_MAX ; i++ ) {
		serialized_layers[ i ].Deserialize( buf.ReadString() );
	}
	const auto serialized_overdraw_coords = DeserializeTileVertices( buf.ReadString() );
	tile_indices_t serialized_overdraw_indices = {};
	serialized_overdraw_indices.Deserialize( buf.ReadString() );
	tile_surfaces_t serialized_overdraw_surfaces = {};
	serialized_overdraw_surfaces.Deserialize( buf.ReadString() );
	const auto serialized_data_coords = DeserializeTileVertices( buf.ReadString() );
	tile_indices_t serialized_data_indices = {};
	serialized_data_indices.Deserialize( buf.ReadString() );
	const auto serialized_has_water = buf.ReadBool();
	const auto serialized_is_coastline_corner = buf.ReadBool();

	const auto w = s_consts.tc.texture_pcx.dimensions.x;
	const auto h = s_consts.tc.texture_pcx.dimensions.y;

	auto serialized_moisture = std::make_unique< types::texture::Texture >( "MoistureOriginal", w, h );
	serialized_moisture->Deserialize( buf.ReadString() );
	std::unique_ptr< types::texture::Texture > serialized_river = nullptr;
	const bool has_river_original = buf.ReadBool();
	if ( has_river_original ) {
		serialized_river = std::make_unique< types::texture::Texture >( "RiverOriginal", w, h );
		serialized_river->Deserialize( buf.ReadString() );
	}

	const size_t sprites_count = buf.ReadCollectionSize( "tile sprite" );
	sprites_t serialized_sprites = {};
	for ( size_t i = 0 ; i < sprites_count ; i++ ) {
		sprite_t sprite;
		sprite.actor = buf.ReadString();
		sprite.instance = buf.ReadInt< size_t >( "tile sprite instance id" );
		sprite.name = buf.ReadString();
		sprite.tex_coords = buf.ReadVec2u();
		if ( sprite.actor.empty() || sprite.name.empty() || sprite.instance == 0 ) {
			THROW( "invalid serialized tile sprite" );
		}
		serialized_sprites.push_back( std::move( sprite ) );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile state" );
	}

	coord = serialized_coord;
	tex_coord = { tex_x, tex_y, tex_x1, tex_y1, tex_x2, tex_y2 };
	elevations = serialized_elevations;
	for ( auto i = 0 ; i < LAYER_MAX ; i++ ) {
		layers[ i ] = serialized_layers[ i ];
	}
	overdraw_column.coords = serialized_overdraw_coords;
	overdraw_column.indices = serialized_overdraw_indices;
	overdraw_column.surfaces = serialized_overdraw_surfaces;
	data_mesh.coords = serialized_data_coords;
	data_mesh.indices = serialized_data_indices;
	has_water = serialized_has_water;
	is_coastline_corner = serialized_is_coastline_corner;
	if ( moisture_original ) {
		DELETE( moisture_original );
	}
	if ( river_original ) {
		DELETE( river_original );
	}
	moisture_original = serialized_moisture.release();
	river_original = serialized_river.release();
	sprites = std::move( serialized_sprites );
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

const tile_vertices_t TileState::DeserializeTileVertices( types::Buffer buf ) {
	const auto center = buf.ReadVec3();
	const auto left = buf.ReadVec3();
	const auto top = buf.ReadVec3();
	const auto right = buf.ReadVec3();
	const auto bottom = buf.ReadVec3();
	if ( !IsFinite( center ) || !IsFinite( left ) || !IsFinite( top ) || !IsFinite( right ) || !IsFinite( bottom ) ) {
		THROW( "serialized tile vertices contain a non-finite value" );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile vertices" );
	}
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

const tile_tex_coords_t TileState::DeserializeTileTexCoords( types::Buffer buf ) {
	const auto center = buf.ReadVec2f();
	const auto left = buf.ReadVec2f();
	const auto top = buf.ReadVec2f();
	const auto right = buf.ReadVec2f();
	const auto bottom = buf.ReadVec2f();
	if ( !IsFinite( center ) || !IsFinite( left ) || !IsFinite( top ) || !IsFinite( right ) || !IsFinite( bottom ) ) {
		THROW( "serialized tile texture coordinates contain a non-finite value" );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile texture coordinates" );
	}
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

void TileState::DeserializeTileColors( types::Buffer buf, tile_colors_t& colors ) {
	tile_colors_t serialized_colors;
	buf.ReadColor( serialized_colors.center );
	buf.ReadColor( serialized_colors.left );
	buf.ReadColor( serialized_colors.top );
	buf.ReadColor( serialized_colors.right );
	buf.ReadColor( serialized_colors.bottom );
	if (
		!IsFinite( serialized_colors.center ) ||
		!IsFinite( serialized_colors.left ) ||
		!IsFinite( serialized_colors.top ) ||
		!IsFinite( serialized_colors.right ) ||
		!IsFinite( serialized_colors.bottom )
	) {
		THROW( "serialized tile colors contain a non-finite value" );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile colors" );
	}
	colors.Set( serialized_colors );
}

void TileState::tile_elevations_t::Deserialize( types::Buffer buf ) {
	const auto serialized_center = buf.ReadInt< elevation_t >( "tile-state center elevation" );
	const auto serialized_left = buf.ReadInt< elevation_t >( "tile-state left elevation" );
	const auto serialized_top = buf.ReadInt< elevation_t >( "tile-state top elevation" );
	const auto serialized_right = buf.ReadInt< elevation_t >( "tile-state right elevation" );
	const auto serialized_bottom = buf.ReadInt< elevation_t >( "tile-state bottom elevation" );
	if (
		serialized_center < ELEVATION_MIN || serialized_center > ELEVATION_MAX ||
		serialized_left < ELEVATION_MIN || serialized_left > ELEVATION_MAX ||
		serialized_top < ELEVATION_MIN || serialized_top > ELEVATION_MAX ||
		serialized_right < ELEVATION_MIN || serialized_right > ELEVATION_MAX ||
		serialized_bottom < ELEVATION_MIN || serialized_bottom > ELEVATION_MAX
	) {
		THROW( "invalid serialized tile-state elevation" );
	}
	if ( serialized_center != ( serialized_left + serialized_top + serialized_right + serialized_bottom ) / 4 ) {
		THROW( "serialized tile-state center elevation does not match its corners" );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile elevations" );
	}
	center = serialized_center;
	left = serialized_left;
	top = serialized_top;
	right = serialized_right;
	bottom = serialized_bottom;
}

void TileState::tile_layer_t::Deserialize( types::Buffer buf ) {
	const auto serialized_coords = DeserializeTileVertices( buf.ReadString() );
	tile_indices_t serialized_indices = {};
	serialized_indices.Deserialize( buf.ReadString() );
	tile_surfaces_t serialized_surfaces = {};
	serialized_surfaces.Deserialize( buf.ReadString() );
	const auto serialized_tex_coords = DeserializeTileTexCoords( buf.ReadString() );
	tile_colors_t serialized_colors;
	DeserializeTileColors( buf.ReadString(), serialized_colors );
	const auto serialized_texture_stretch = buf.ReadVec2f();
	const auto serialized_texture_stretch_at_edges = buf.ReadBool();
	if ( !IsFinite( serialized_texture_stretch ) ) {
		THROW( "serialized tile layer contains non-finite texture stretch" );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile layer" );
	}
	coords = serialized_coords;
	indices = serialized_indices;
	surfaces = serialized_surfaces;
	tex_coords = serialized_tex_coords;
	colors.Set( serialized_colors );
	texture_stretch = serialized_texture_stretch;
	texture_stretch_at_edges = serialized_texture_stretch_at_edges;
}

void TileState::tile_indices_t::Deserialize( types::Buffer buf ) {
	const auto serialized_center = buf.ReadInt< types::mesh::index_t >( "tile center vertex index" );
	const auto serialized_left = buf.ReadInt< types::mesh::index_t >( "tile left vertex index" );
	const auto serialized_top = buf.ReadInt< types::mesh::index_t >( "tile top vertex index" );
	const auto serialized_right = buf.ReadInt< types::mesh::index_t >( "tile right vertex index" );
	const auto serialized_bottom = buf.ReadInt< types::mesh::index_t >( "tile bottom vertex index" );
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile indices" );
	}
	center = serialized_center;
	left = serialized_left;
	top = serialized_top;
	right = serialized_right;
	bottom = serialized_bottom;
}

void TileState::tile_surfaces_t::Deserialize( types::Buffer buf ) {
	const auto serialized_left_top = buf.ReadInt< types::mesh::surface_id_t >( "tile left-top surface id" );
	const auto serialized_top_right = buf.ReadInt< types::mesh::surface_id_t >( "tile top-right surface id" );
	const auto serialized_right_bottom = buf.ReadInt< types::mesh::surface_id_t >( "tile right-bottom surface id" );
	const auto serialized_bottom_left = buf.ReadInt< types::mesh::surface_id_t >( "tile bottom-left surface id" );
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized tile surfaces" );
	}
	left_top = serialized_left_top;
	top_right = serialized_top_right;
	right_bottom = serialized_right_bottom;
	bottom_left = serialized_bottom_left;
}

}
}
}
}
