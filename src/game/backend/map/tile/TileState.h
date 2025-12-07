#pragma once

#include "Types.h"
#include "game/backend/map/Types.h"
#include "types/mesh/Types.h"
#include "types/Buffer.h"
#include "types/Vec2.h"
#include "types/Vec3.h"
#include "types/Color.h"

namespace types::texture {
class Texture;
}

namespace game {
namespace backend {
namespace map {
namespace tile {

class TileState {
public:

	~TileState();

	// links to neighbours
	TileState* W;
	TileState* NW;
	TileState* N;
	TileState* NE;
	TileState* E;
	TileState* SE;
	TileState* S;
	TileState* SW;
	TileState* GetNeighbour( const direction_t direction );

	types::Vec2< float > coord;
	struct {
		float x;
		float y;
		float x1;
		float y1;
		float x2;
		float y2;
	} tex_coord;

	struct tile_indices_t {
		types::mesh::index_t center;
		types::mesh::index_t left;
		types::mesh::index_t right;
		types::mesh::index_t top;
		types::mesh::index_t bottom;
		const types::Buffer Serialize() const;
		void Deserialize( types::Buffer buf );
	};

	struct tile_surfaces_t {
		types::mesh::surface_id_t left_top;
		types::mesh::surface_id_t top_right;
		types::mesh::surface_id_t right_bottom;
		types::mesh::surface_id_t bottom_left;
		const types::Buffer Serialize() const;
		void Deserialize( types::Buffer buf );
	};

	struct tile_layer_t {
		tile_vertices_t coords;
		tile_indices_t indices;
		tile_surfaces_t surfaces;
		tile_tex_coords_t tex_coords;
		tile_colors_t colors;
		types::Vec2< types::mesh::coord_t > texture_stretch; // each tile has only one 'own' stretch value (for bottom vertex), others are copied from neighbours
		bool texture_stretch_at_edges;
		const types::Buffer Serialize() const;
		void Deserialize( types::Buffer buf );
	};

	struct tile_elevations_t {
		elevation_t center;
		elevation_t left;
		elevation_t top;
		elevation_t right;
		elevation_t bottom;
		const types::Buffer Serialize() const;
		void Deserialize( types::Buffer buf );
	};

	tile_elevations_t elevations;

	tile_layer_t layers[LAYER_MAX];

	struct {
		tile_vertices_t coords;
		tile_indices_t indices;
		tile_surfaces_t surfaces;
	} overdraw_column; // need to copy first column after last one to make blending and light compute correctly in instancing
	struct {
		tile_vertices_t coords;
		tile_indices_t indices;
	} data_mesh;

	// visual traits
	bool has_water;
	bool is_coastline_corner;

	types::texture::Texture* moisture_original = nullptr;
	types::texture::Texture* river_original = nullptr;

	// bonus resources, supply pods and terraforming (except for roads/tubes)
	typedef struct {
		std::string actor;
		size_t instance;
		std::string name;
		pcx_texture_coordinates_t tex_coords;
	} sprite_t;

	typedef std::vector< sprite_t > sprites_t;

	sprites_t sprites;

	const types::Vec3& GetCenterCoords( tile_layer_type_t layer ) const;

	const types::Buffer Serialize() const;
	void Deserialize( types::Buffer buf );

private:
	static const types::Buffer SerializeTileVertices( const tile_vertices_t& vertices );
	static const tile_vertices_t DeserializeTileVertices( types::Buffer buf );
	static const types::Buffer SerializeTileTexCoords( const tile_tex_coords_t& tex_coords );
	static const tile_tex_coords_t DeserializeTileTexCoords( types::Buffer buf );
	static const types::Buffer SerializeTileColors( const tile_colors_t& colors );
	static const tile_colors_t DeserializeTileColors( types::Buffer buf );
};

}
}
}
}
