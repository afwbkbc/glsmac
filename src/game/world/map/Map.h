#pragma once

// TODO: merge this class with World?

#include <vector>
#include <unordered_map>

#include "types/Serializable.h"

#include "Tiles.h"

#include "util/Clamper.h"

#include "types/Texture.h"

#include "scene/Scene.h"
#include "scene/actor/Sprite.h"
#include "scene/actor/Mesh.h"
#include "scene/actor/Instanced.h"

using namespace types;
using namespace scene;

namespace game {
namespace world {
namespace map {

class Module;
class Finalize;
	
CLASS( Map, Serializable )

	Map( Random* random, Scene* scene );
	~Map();
	
	// coordinates of textures (x1 and y1) in texture.pcx
	typedef Vec2< uint32_t > pcx_texture_coordinates_t;
	
	struct consts_t {
		const struct {
			const struct {
				
				const Vec2< size_t > dimensions = { 56, 56 };
				
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
					{ 280, 516 }, { 337, 516 }, { 394, 516 }, { 451, 516 },
					{ 280, 573 }, { 337, 573 }, { 394, 573 }, { 451, 573 },
					{ 280, 630 }, { 337, 630 }, { 394, 630 }, { 451, 630 },
					{ 280, 687 }, { 337, 687 }, { 394, 687 }, { 451, 687 },
				};
				const pcx_texture_coordinates_t fungus_sea[16] = {
					{ 508, 516 }, { 565, 516 }, { 622, 516 }, { 679, 516 },
					{ 508, 573 }, { 565, 573 }, { 622, 573 }, { 679, 573 },
					{ 508, 630 }, { 565, 630 }, { 622, 630 }, { 679, 630 },
					{ 508, 687 }, { 565, 687 }, { 622, 687 }, { 679, 687 },
				};
				const pcx_texture_coordinates_t river[16] = {
					{ 280, 259 }, { 337, 259 }, { 394, 259 }, { 451, 259 },
					{ 280, 316 }, { 337, 316 }, { 394, 316 }, { 451, 316 },
					{ 280, 373 }, { 337, 373 }, { 394, 373 }, { 451, 373 },
					{ 280, 430 }, { 337, 430 }, { 394, 430 }, { 451, 430 },
				};
				
			} texture_pcx;
			const struct {
				
				const Vec2< size_t > dimensions = { 100, 62 };
				
				// some coordinates were altered to fix alignment
				const pcx_texture_coordinates_t nutrient_bonus_sea[ 2 ] = {
					{ 4, 257 }, { 106, 257 },
					//{ 1, 253 }, { 103, 253 },
				};
				const pcx_texture_coordinates_t nutrient_bonus_land[ 2 ] = {
					{ 207, 257 }, { 308, 257 },
					//{ 203, 253 }, { 304, 253 },
				};
				const pcx_texture_coordinates_t minerals_bonus_sea[ 2 ] = {
					{ 1, 316 }, { 102, 316 },
				};
				const pcx_texture_coordinates_t minerals_bonus_land[ 2 ] = {
					{ 203, 316 }, { 304, 316 },
					//{ 203, 316 }, { 304, 316 },
				};
				const pcx_texture_coordinates_t energy_bonus_sea[ 2 ] = {
					{ 1, 379 }, { 102, 379 },
				};
				const pcx_texture_coordinates_t energy_bonus_land[ 2 ] = {
					{ 203, 379 }, { 304, 379 },
				};
				
			} ter1_pcx;
		} tc;
		const float tile_scale_z = 2.0f;
		const struct {
			const struct {
				const float x = 1.0f;
				const float y = 1.0f;
				const float z = s_consts.tile_scale_z;
			} scale;
			const struct {
				const float x =	s_consts.tile.scale.x / 2;
				const float y =	s_consts.tile.scale.y / 2;
			} radius;
			const float rotated_width = sqrt( pow( s_consts.tile.scale.x, 2 ) + pow( s_consts.tile.scale.y, 2 ) );
			const Tile::elevation_t maximum_allowed_slope_elevation = 650; // TODO: fix black lines when texture is perpendicular to camera
			const struct {
				const float texture_edge_stretch_min = 0.05f;
				const float texture_edge_stretch_max = 0.15f;
				const float texture_center_stretch = 4.0f;
				const float center_coordinates_shift = 2.0f;
			} random;
		} tile;
		const struct {
			const Vec2< size_t > dimensions = s_consts.tc.texture_pcx.dimensions;
			const Vec2< size_t > radius = s_consts.tile_texture.dimensions / (size_t)2;
		} tile_texture;
		const struct {
			const float zoom = 0.005f;
			// to compensate for view angle difference from SMAC's
			const float y_scale = 0.8f;
			const float y_shift = 0.12f;
		} sprite;
		const Vec3 map_position = { 0.0f, 0.0f, 0.0f };
		const Vec3 map_rotation = { 0.0f, 0.0f, 0.0f };
		const Color underwater_tint = { 0.0f, 0.2f, 0.5f, 1.0f };
		const struct {
			//const Color coastline_tint = { 0.7f, 0.7f, 0.7f, 1.0f };
			const Color coastline_tint = { 1.0f, 1.0f, 1.0f, 1.0f };
			const float coast_water_alpha = 0.4f;
			const float coast_water_center_alpha = coast_water_alpha / 1.5f;
			const float coast_water_center_alpha_corner_mod = 0.7f;
			const Color border_color = { 0.425f, 0.378f, 0.311f, 1.0f };
			const float border_alpha = 0.7f;
			const float border_size = 6.0f;
			const struct {
				const float range = 0.35f;
				const float frequency = 0.15f;
				const uint8_t passes = 4;
				const float cut = 0.3f;
				const float round_range = 2.0f;
			} perlin;
		} coastlines;
		const struct {
			const util::Clamper< float > elevation_to_vertex_z = { Tile::ELEVATION_MIN, Tile::ELEVATION_MAX, -s_consts.tile_scale_z, s_consts.tile_scale_z };
			const util::Clamper< float > elevation_to_water_r = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.6f, 1.3f };
			const util::Clamper< float > elevation_to_water_g = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.6f, 1.8f };
			const util::Clamper< float > elevation_to_water_b = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.8f, 1.8f };
			const util::Clamper< float > elevation_to_water_a = { Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 1.0f, 0.5f };
		} clampers;
		const struct {
			const float water = s_consts.clampers.elevation_to_vertex_z.Clamp( Tile::ELEVATION_LEVEL_COAST ); // sea is always on sea level
		} levels;
	};
	static const consts_t s_consts;
	
	void SetTiles( Tiles* tiles, bool generate_actors = true );
	
#ifdef DEBUG
	std::vector<scene::actor::Instanced*> GetActors() const;
#endif
	
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
		tile_indices_t indices;
		tile_vertices_t coords;
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
	
	struct tile_state_t {
		
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
		tile_state_t* W;
		tile_state_t* NW;
		tile_state_t* N;
		tile_state_t* NE;
		tile_state_t* E;
		tile_state_t* SE;
		tile_state_t* S;
		tile_state_t* SW;
		
		// coordinates etc
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
		
		// visual traits
		bool has_water;
		bool is_coastline_corner;
		Texture* moisture_original;
		Texture* river_original;
		
		// bonus resources, supply pods and terraforming (except for roads/tubes)
		typedef struct {
			scene::actor::Instanced* actor;
			pcx_texture_coordinates_t tex_coords;
		} sprite_t;
		typedef std::vector< sprite_t > sprites_t;
		sprites_t sprites;
		
		const Buffer Serialize() const;
		void Unserialize( const Map* map, Buffer buf );
		
	};
	
	struct copy_from_after_t {
		Texture::add_flag_t mode;
		size_t tx1_from;
		size_t ty1_from;
		size_t tx2_from;
		size_t ty2_from;
		size_t tx_to;
		size_t ty_to;
		uint8_t rotate;
		float alpha;
		util::Perlin* perlin = nullptr;
	};
	
	struct map_state_t {
		Vec2< float > coord;
		struct {
			Vec2< float > min;
			Vec2< float > max;
			struct {
				util::Clamper< float > x;
				util::Clamper< float > y;
			} percent_to_absolute;
		} range;
		Vec2< uint32_t > dimensions;
		struct {
			Vec2< float > texture_scaling;
		} variables;
		std::vector< copy_from_after_t > copy_from_after;

		const Texture* terrain_texture;
		const Texture* ter1_pcx;
		
		const Buffer Serialize() const;
		void Unserialize( Buffer buf );
	};
	
	// call these only during tile generation
	void AddTexture( const tile_layer_type_t tile_layer, const pcx_texture_coordinates_t& tc, const Texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void CopyTextureFromLayer( const tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from, const tile_layer_type_t tile_layer, const Texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void CopyTexture( const tile_layer_type_t tile_layer_from, const tile_layer_type_t tile_layer, const Texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void CopyTextureDeferred( const tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from,const tile_layer_type_t tile_layer, const Texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void GetTexture( Texture* dest_texture, const pcx_texture_coordinates_t& tc, const Texture::add_flag_t mode, const uint8_t rotate = 0, const float alpha = 1.0f );
	void GetTextureFromLayer( Texture* dest_texture, const tile_layer_type_t tile_layer, const size_t tx_from, const size_t ty_from, const Texture::add_flag_t mode = Texture::AM_DEFAULT, const uint8_t rotate = 0, const float alpha = 1.0f ) const;
	void SetTexture( const tile_layer_type_t tile_layer, tile_state_t* ts, Texture* src_texture, const Texture::add_flag_t mode, const uint8_t rotate = 0, const float alpha = 1.0f );
	void SetTexture( const tile_layer_type_t tile_layer, Texture* src_texture, const Texture::add_flag_t mode, const uint8_t rotate = 0, const float alpha = 1.0f );
	
	void SetMinimapColor( const Color& color );
	
	const Texture* GetTerrainTexture() const;
	
	enum tile_grouping_criteria_t {
		TG_MOISTURE,
		TG_FEATURE,
	};
	struct tile_texture_info_t {
		uint8_t rotate_direction;
		uint8_t texture_variant;
		Texture::add_flag_t texture_flags;
	};
	
	enum texture_variants_type_t {
		TVT_NONE,
		TVT_TILES,
		TVT_RIVERS,
		TVT_ROADS
	};
	const tile_texture_info_t GetTileTextureInfo( const texture_variants_type_t type, const Tile* tile, const tile_grouping_criteria_t criteria, const Tile::feature_t feature = Tile::F_NONE ) const;
	
	const size_t GetWidth() const;
	const size_t GetHeight() const;
	
	Random* GetRandom() const;
	
	struct tile_info_t {
		const Tile* tile;
		const tile_state_t* ts;
		const map_state_t* ms;
	};
	const tile_info_t GetTileAt( const size_t tile_x, const size_t tile_y ) const;
	
	// tile request stuff
	void CancelTileAtRequest();
	void GetTileAtScreenCoords( const size_t screen_x, const size_t screen_inverse_y ); // async, y needs to be upside down
	tile_info_t GetTileAtScreenCoordsResult();
	
	// for minimap
	void GetMinimapTexture( scene::Camera* camera, const Vec2< size_t > texture_dimensions );
	Texture* GetMinimapTextureResult();
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
	
	tile_state_t* GetTileState( const size_t x, const size_t y ) const;
	tile_state_t* GetTileState( const Tile* tile ) const;
	const map_state_t* GetMapState() const;
	
	Scene* GetScene() const;
	
	scene::actor::Instanced* GenerateTerrainSpriteActor( const std::string& name, const pcx_texture_coordinates_t& tex_coords ) const;
	
protected:
	friend class Finalize;
	
	types::mesh::Render* m_mesh_terrain = nullptr;
	types::mesh::Data* m_mesh_terrain_data = nullptr;
	
private:
	
	rr::id_t m_tile_at_request_id = 0;
	rr::id_t m_minimap_texture_request_id = 0;
	
	tile_state_t* m_tile_states = nullptr;
	
	map_state_t m_map_state;
	
	Tiles* m_tiles = nullptr;
	
	struct {
		struct {
			Texture* texture_pcx = nullptr;
			Texture* ter1_pcx = nullptr;
		} source;
		Texture* terrain = nullptr;
		Texture* minimap = nullptr;
	} m_textures;
	
	Random* m_random = nullptr;
	
	Scene* m_scene = nullptr;
	struct {
		scene::actor::Instanced* terrain = nullptr;
	} m_actors;
	
	typedef std::vector< Module* > module_pass_t;
	typedef std::vector< module_pass_t > module_passes_t;
	module_passes_t m_modules; // before finalizing and deferred calls
	module_passes_t m_modules_deferred; // after finalizing and deferred calls
	
	void LinkTileStates();
	void RunModulePasses( module_passes_t& module_passes );
	void InitTextureAndMesh();
	void InitTerrainActor();
	void GenerateActors();
	
	// texture.pcx contains some textures grouped in certain way based on adjactent neighbours
	// calculate all variants once and cache for faster lookups later
	struct texture_rule_t {
		uint8_t bitmask; // presence of nearby neighbours ( 1 bit per neighbour )
		uint8_t checked_bits; // some masks don't care about some corners
	};
	typedef std::vector< texture_rule_t > texture_variants_rules_t;
	typedef std::unordered_map< uint8_t, std::vector< uint8_t > > texture_variants_t; // neighbours bitmask -> texture variants (can be multiple)
	std::unordered_map< texture_variants_type_t, texture_variants_t > m_texture_variants = {};
	void CalculateTextureVariants( const texture_variants_type_t type, const texture_variants_rules_t& rules );
	
	tile_state_t* m_current_ts = nullptr;
	Tile* m_current_tile = nullptr;

};

}
}
}
