#pragma once

#include "types/Serializable.h"

#include "Consts.h"
#include "Tile.h"
#include "types/mesh/Mesh.h"
#include "types/Texture.h"
#include "scene/actor/Instanced.h"

using namespace types;

namespace game {
namespace world {

// do not extend any class because tile states are allocated with malloc
class TileState {
public:

	// coordinates in world
	Vec2< float > coord;

	struct {
		float x;
		float y;
		float x1;
		float y1;
		float x2;
		float y2;
	} tex_coord;

	// links to neighbours
	TileState* W;
	TileState* NW;
	TileState* N;
	TileState* NE;
	TileState* E;
	TileState* SE;
	TileState* S;
	TileState* SW;

	// tile data

	// order is important (it defines rendering order)
	enum tile_layer_type_t {
		LAYER_LAND, // land tiles, includes edge-smoothed moisture textures
		LAYER_WATER_SURFACE, // water tiles (alpha surface with gradients)
		LAYER_WATER_SURFACE_EXTRA, // extra textures/effects for water tiles
		LAYER_WATER, // sea (for non-alpha objects on water)
		LAYER_MAX
	};

	struct tile_vertices_t {
		Vec3 center;
		Vec3 left;
		Vec3 top;
		Vec3 right;
		Vec3 bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	};

	struct tile_indices_t {
		types::mesh::Mesh::index_t center;
		types::mesh::Mesh::index_t left;
		types::mesh::Mesh::index_t right;
		types::mesh::Mesh::index_t top;
		types::mesh::Mesh::index_t bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	};

	struct tile_surfaces_t {
		types::mesh::Mesh::surface_id_t left_top;
		types::mesh::Mesh::surface_id_t top_right;
		types::mesh::Mesh::surface_id_t right_bottom;
		types::mesh::Mesh::surface_id_t bottom_left;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	};

	struct tile_tex_coords_t {
		Vec2< float > center;
		Vec2< float > left;
		Vec2< float > top;
		Vec2< float > right;
		Vec2< float > bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	};

	struct tile_colors_t {
		Color center;
		Color left;
		Color top;
		Color right;
		Color bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	};

	struct tile_layer_t {
		tile_vertices_t coords;
		tile_indices_t indices;
		tile_surfaces_t surfaces;
		tile_tex_coords_t tex_coords;
		tile_colors_t colors;
		Vec2< mesh::Mesh::coord_t > texture_stretch; // each tile has only one 'own' stretch value (for bottom vertex), others are copied from neighbours
		bool texture_stretch_at_edges;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	};

	struct tile_elevations_t {
		Tile::elevation_t center;
		Tile::elevation_t left;
		Tile::elevation_t top;
		Tile::elevation_t right;
		Tile::elevation_t bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
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

	Texture* moisture_original;
	Texture* river_original;

	// bonus resources, supply pods and terraforming (except for roads/tubes)
	typedef struct {
		std::string actor;
		size_t instance;
		std::string name;
		Consts::pcx_texture_coordinates_t tex_coords;
	} sprite_t;

	typedef std::vector< sprite_t > sprites_t;

	sprites_t sprites;

	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
};

}
}

