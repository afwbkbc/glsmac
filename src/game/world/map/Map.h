#pragma once

#include "types/Serializable.h"

#include <vector>
#include <unordered_map>

#include "Tiles.h"

#include "util/Clamper.h"

#include "types/Texture.h"
#include "scene/actor/Mesh.h"
#include "scene/Scene.h"

using namespace types;
using namespace scene;

// for development/debug only
#define WIREFRAMES 0
#define WIREFRAMES_GRADIENTS 0

namespace game {
namespace world {
namespace map {

class Module;
class Finalize;
	
CLASS( Map, Serializable )

	Map( Random* random, Scene* scene );
	~Map();
	
	// coordinates of textures (x1 and y1) in texture.pcx
	typedef Vec2< size_t > pcx_texture_coordinates_t;
	
	typedef struct {
		const struct {
			const pcx_texture_coordinates_t water[2] = {
				{ 280, 79 }, {280, 136 },
			};
			const pcx_texture_coordinates_t sunny_mesa[8] = {
				{ 768, 15 }, { 825, 15 }, { 882, 15 }, { 939, 15 },
				{ 768, 72 }, { 825, 72 }, { 882, 72 }, { 939, 72 },
			};
			const pcx_texture_coordinates_t rocks[4] = {
				{ 1, 1 }, { 58, 1 }, { 115, 1 }, { 172, 1 }
			};
			const pcx_texture_coordinates_t dunes[1] = {
				{ 229, 1 }
			};
			const pcx_texture_coordinates_t arid[1] = {
				{ 1, 58 }
			};
			const pcx_texture_coordinates_t moist[16] = {
				{ 1, 115 }, { 58, 115 }, { 115, 115 }, { 172, 115 },
				{ 1, 172 }, { 58, 172 }, { 115, 172 }, { 172, 172 },
				{ 1, 229 }, { 58, 229 }, { 115, 229 }, { 172, 229 },
				{ 1, 286 }, { 58, 286 }, { 115, 286 }, { 172, 286 },
			};
			const pcx_texture_coordinates_t rainy[16] = {
				{ 1, 343 }, { 58, 343 }, { 115, 343 }, { 172, 343 },
				{ 1, 400 }, { 58, 400 }, { 115, 400 }, { 172, 400 },
				{ 1, 457 }, { 58, 457 }, { 115, 457 }, { 172, 457 },
				{ 1, 514 }, { 58, 514 }, { 115, 514 }, { 172, 514 },
			};
			const pcx_texture_coordinates_t jungle[16] = {
				{ 526, 259 }, { 583, 259 }, { 640, 259 }, { 697, 259 },
				{ 526, 316 }, { 583, 316 }, { 640, 316 }, { 697, 316 },
				{ 526, 373 }, { 583, 373 }, { 640, 373 }, { 697, 373 },
				{ 526, 430 }, { 583, 430 }, { 640, 430 }, { 959, 463 },
			};
			const pcx_texture_coordinates_t fungus_land[16] = {
				{ 280, 515 }, { 337, 515 }, { 394, 515 }, { 451, 515 },
				{ 280, 572 }, { 337, 572 }, { 394, 572 }, { 451, 572 },
				{ 280, 629 }, { 337, 629 }, { 394, 629 }, { 451, 629 },
				{ 280, 686 }, { 337, 686 }, { 394, 686 }, { 451, 686 },
			};
			const pcx_texture_coordinates_t fungus_sea[16] = {
				{ 508, 515 }, { 565, 515 }, { 622, 515 }, { 679, 515 },
				{ 508, 572 }, { 565, 572 }, { 622, 572 }, { 679, 572 },
				{ 508, 629 }, { 565, 629 }, { 622, 629 }, { 679, 629 },
				{ 508, 686 }, { 565, 686 }, { 622, 686 }, { 679, 686 },
			};
		} pcx_textures;
		const float tile_scale_z = 2.0f;
		const struct {
			const struct {
				const float x = 1.0f;
				const float y = 1.0f;
				const float z = Map::s_consts.tile_scale_z;
			} scale;
			const struct {
				const float x =	Map::s_consts.tile.scale.x / 2;
				const float y =	Map::s_consts.tile.scale.y / 2;
			} radius;
			const float rotated_width = sqrt( pow( Map::s_consts.tile.scale.x, 2 ) + pow( Map::s_consts.tile.scale.y, 2 ) );
			const Tile::elevation_t maximum_allowed_slope_elevation = 1000;
		} tile;
		const struct {
			const struct {
				const uint8_t x = 56;
				const uint8_t y = 56;
			} dimensions;
			const struct {
				const uint8_t x = Map::s_consts.pcx_texture_block.dimensions.x / 2;
				const uint8_t y = Map::s_consts.pcx_texture_block.dimensions.y / 2;
			} radius;
		} pcx_texture_block;
		const Vec3 map_position = { 0.0f, 0.0f, 0.0f };
		const Vec3 map_rotation = { 0.0f, 0.0f, 0.0f };
		const Color underwater_tint = { 0.0f, 0.2f, 0.5f, 1.0f };
		const struct {
			const Color coastline_tint = { 0.7f, 0.7f, 0.7f, 1.0f };
			const Color::channel_t coast_water_alpha = 0.3f;
			const float coast_water_center_alpha = 0.5f;
			const float coast_water_center_alpha_tint_mod = 0.75f;
		} coastlines;
		const struct {
			const util::Clamper< float > elevation_to_vertex_z = { Tile::ELEVATION_MIN, Tile::ELEVATION_MAX, -Map::s_consts.tile_scale_z, Map::s_consts.tile_scale_z };
			const util::Clamper< float > elevation_to_water_r = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.6f, 1.3f };
			const util::Clamper< float > elevation_to_water_g = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.6f, 1.8f };
			const util::Clamper< float > elevation_to_water_b = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.8f, 1.8f };
			const util::Clamper< float > elevation_to_water_a = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 1.0f, 0.5f };
		} clampers;
		const struct {
			const float water = Map::s_consts.clampers.elevation_to_vertex_z.Clamp( Tile::ELEVATION_LEVEL_COAST ); // sea is always on sea level
		} levels;
	} consts_t;
	static const consts_t s_consts;
	
	void SetTiles( Tiles* tiles, bool generate_actors = true );
	
#ifdef DEBUG
	vector<scene::actor::Mesh*> GetActors() const;
#endif
	
	// order is important (it defines rendering order)
	enum tile_layer_type_t {
		LAYER_LAND, // land tiles
		LAYER_WATER_SURFACE, // water tiles (alpha surface with gradients)
		LAYER_WATER_SURFACE_EXTRA, // extra textures/effects for water tiles
		LAYER_WATER, // sea (for non-alpha objects on water)
		LAYER_MAX
	};

	typedef struct {
		Vec3 center;
		Vec3 left;
		Vec3 top;
		Vec3 right;
		Vec3 bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} tile_vertices_t;

	typedef struct {
		types::mesh::Mesh::index_t center;
		types::mesh::Mesh::index_t left;
		types::mesh::Mesh::index_t right;
		types::mesh::Mesh::index_t top;
		types::mesh::Mesh::index_t bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} tile_indices_t;
	
	typedef struct {
		Vec2< float > center;
		Vec2< float > left;
		Vec2< float > top;
		Vec2< float > right;
		Vec2< float > bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} tile_tex_coords_t;
	
	typedef struct {
		Color center;
		Color left;
		Color top;
		Color right;
		Color bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} tile_colors_t;
	
	typedef struct {
		tile_indices_t indices;
		tile_vertices_t coords;
		tile_tex_coords_t tex_coords;
		tile_colors_t colors;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} tile_layer_t;
	
	typedef struct {
		Tile::elevation_t center;
		Tile::elevation_t left;
		Tile::elevation_t top;
		Tile::elevation_t right;
		Tile::elevation_t bottom;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} tile_elevations_t;
	
	typedef struct {
		// world coordinates
		struct {
			float x;
			float y;
		} coord;
		struct {
			float x;
			float y;
			float x1;
			float y1;
			float x2;
			float y2;
		} tex_coord;
		tile_elevations_t elevations;
		tile_layer_t layers[ LAYER_MAX ];
		struct {
			tile_vertices_t coords;
			tile_indices_t indices;
		} overdraw_column; // need to copy first column after last one to make blending and light compute correctly in instancing
		struct {
			tile_vertices_t coords;
			tile_indices_t indices;
		} data_mesh;
		bool is_coastline_corner;
		bool has_water;
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} tile_state_t;
	
	typedef struct {
		Texture::add_mode_t mode;
		size_t tx1_from;
		size_t ty1_from;
		size_t tx2_from;
		size_t ty2_from;
		size_t tx_to;
		size_t ty_to;
		uint8_t rotate;
		float alpha;
	} copy_from_after_t;
	
	typedef struct {
		struct {
			float x;
			float y;
		} coord;
		struct {
			size_t x;
			size_t y;
		} dimensions;
		struct {
			struct {
				float x;
				float y;
			} texture_scaling;
		} variables;
		vector< copy_from_after_t > copy_from_after;

		// TODO: refactor this
		vector< pair< types::mesh::Mesh::index_t*, types::mesh::Mesh::index_t* > > copy_normals;
		unordered_map< types::mesh::Mesh::index_t*, pair< Vec2< size_t >, Texture::add_mode_t > > need_normals;
		
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	} map_state_t;
	
	// call these only during tile generation
	void AddTexture( const tile_layer_type_t tile_layer, const pcx_texture_coordinates_t& tc, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f );
	void CopyTextureFromLayer( const tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from, const tile_layer_type_t tile_layer, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f );
	void CopyTexture( const tile_layer_type_t tile_layer_from, const tile_layer_type_t tile_layer, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f );
	void CopyTextureDeferred( const tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from,const tile_layer_type_t tile_layer, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f );
	enum tile_grouping_criteria_t {
		TG_MOISTURE,
		TG_FEATURE,
	};
	typedef struct {
		uint8_t rotate_direction;
		uint8_t texture_variant;
	} tile_texture_info_t;
	const tile_texture_info_t GetTileTextureInfo( const Tile* tile, const tile_grouping_criteria_t criteria, const Tile::feature_t feature = Tile::F_NONE ) const;

	const size_t GetWidth() const;
	const size_t GetHeight() const;
	
	Random* GetRandom() const;
	
	typedef struct {
		const Tile* tile;
		const tile_state_t* ts;
		const map_state_t* ms;
	} tile_info_t;
	const tile_info_t GetTileAt( const size_t tile_x, const size_t tile_y ) const;
	
	// tile request stuff
	void CancelTileAtRequest();
	void GetTileAtScreenCoords( const size_t screen_x, const size_t screen_inverse_y ); // async, y needs to be upside down
	tile_info_t GetTileAtScreenCoordsResult();
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
	
protected:
	friend class Finalize;
	
	types::mesh::Render* m_mesh_terrain = nullptr;
	types::mesh::Data* m_mesh_terrain_data = nullptr;
	
private:
	
	scene::actor::Mesh::data_request_id_t m_tile_at_request_id = 0;
	
	tile_state_t* m_tile_states = nullptr;
	tile_state_t* GetTileState( const size_t x, const size_t y ) const;
	
	map_state_t m_map_state;
	
	Tiles* m_tiles = nullptr;
	
	struct {
		Texture* source = nullptr;
		Texture* terrain = nullptr;
	} m_textures;
	
	Random* m_random = nullptr;
	
	Scene* m_scene = nullptr;
	struct {
		scene::actor::Mesh* terrain = nullptr;
	} m_actors;
	
	void GenerateActors();
	
	void InitTextureAndMesh();
	
	unordered_map< uint8_t, uint8_t > m_texture_variants = {}; // cache
	
	tile_state_t* m_current_ts = nullptr;
	Tile* m_current_tile = nullptr;

	vector< Module* > m_modules;
};

}
}
}
