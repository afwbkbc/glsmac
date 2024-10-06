#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "types/Serializable.h"

#include "Types.h"
#include "common/MTTypes.h"
#include "game/backend/map/tile/Types.h"
#include "types/texture/Types.h"

#include "types/Buffer.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Render;
class Data;
}
}

namespace util {
namespace random {
class Random;
}
class Perlin;
}

namespace game {
namespace backend {

class Game;

namespace settings {
class MapSettings;
}

namespace map {

namespace tile {
class Tile;
class TileState;
class Tiles;
}

class MapState;

namespace module {
class Module;

class Finalize;
}

CLASS( Map, types::Serializable )

	Map( Game* game );
	~Map();

	enum error_code_t {
		EC_NONE = 0,
		EC_UNKNOWN = 1,
		EC_ABORTED = 2,
		EC_MAPFILE_FORMAT_ERROR = 3,
	};

	const error_code_t Generate( settings::MapSettings* map_settings, MT_CANCELABLE );
	const error_code_t LoadFromBuffer( types::Buffer& buffer );
	const error_code_t LoadFromFile( const std::string& path );
	void SaveToBuffer( types::Buffer& buffer ) const;
	const error_code_t SaveToFile( const std::string& path ) const;

	const error_code_t Initialize( MT_CANCELABLE );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

	static const std::string& GetErrorString( const error_code_t& code );

	enum tile_grouping_criteria_t {
		TG_MOISTURE,
		TG_FEATURE,
		TG_TERRAFORMING,
	};
	struct tile_texture_info_t {
		uint8_t rotate_direction;
		uint8_t texture_variant;
		types::texture::add_flag_t texture_flags;
	};

	enum texture_variants_type_t {
		TVT_NONE,
		TVT_TILES,
		TVT_RIVERS_FORESTS
	};

	// call these only from map modules during tile generation
	tile::Tile* GetTile( const size_t x, const size_t y ) const;
	tile::TileState* GetTileState( const size_t x, const size_t y ) const;
	tile::TileState* GetTileState( const tile::Tile* tile ) const;
	const MapState* GetMapState() const;
	void ClearTexture();
	void AddTexture( const tile::tile_layer_type_t tile_layer, const pcx_texture_coordinates_t& tc, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void CopyTextureFromLayer( const tile::tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from, const tile::tile_layer_type_t tile_layer, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void CopyTexture( const tile::tile_layer_type_t tile_layer_from, const tile::tile_layer_type_t tile_layer, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void CopyTextureDeferred( const tile::tile_layer_type_t tile_layer_from, const size_t tx_from, const size_t ty_from, const tile::tile_layer_type_t tile_layer, const types::texture::add_flag_t mode, const uint8_t rotate, const float alpha = 1.0f, util::Perlin* perlin = nullptr );
	void GetTexture( types::texture::Texture* dest_texture, const pcx_texture_coordinates_t& tc, const types::texture::add_flag_t mode, const uint8_t rotate = 0, const float alpha = 1.0f );
	void GetTextureFromLayer( types::texture::Texture* dest_texture, const tile::tile_layer_type_t tile_layer, const size_t tx_from, const size_t ty_from, const types::texture::add_flag_t mode = types::texture::AM_DEFAULT, const uint8_t rotate = 0, const float alpha = 1.0f ) const;
	void SetTexture( const tile::tile_layer_type_t tile_layer, tile::TileState* ts, types::texture::Texture* src_texture, const types::texture::add_flag_t mode, const uint8_t rotate = 0, const float alpha = 1.0f );
	void SetTexture( const tile::tile_layer_type_t tile_layer, types::texture::Texture* src_texture, const types::texture::add_flag_t mode, const uint8_t rotate = 0, const float alpha = 1.0f );

	const tile_texture_info_t GetTileTextureInfo( const texture_variants_type_t type, const tile::Tile* tile, const tile_grouping_criteria_t criteria, const tile::feature_t feature = tile::FEATURE_NONE ) const;
	util::random::Random* GetRandom() const;

	const size_t GetWidth() const;
	const size_t GetHeight() const;

	// be careful using this
	tile::Tiles* GetTilesPtr() const;

	const types::Buffer SerializeSpriteActor( const sprite_actor_t& sprite_actor ) const;
	const sprite_actor_t UnserializeSpriteActor( types::Buffer buf ) const;

	const std::string GetTerrainSpriteActor( const std::string& name, const pcx_texture_coordinates_t& tex_coords, const float z_index );
	const size_t AddTerrainSpriteActorInstance( const std::string& key, const types::Vec3& coords );
	void RemoveTerrainSpriteActorInstance( const std::string& key, const size_t instance_id );

private:
	friend class module::Finalize;
	friend class backend::Game;

	struct {
		types::mesh::Render* terrain = nullptr;
		types::mesh::Data* terrain_data = nullptr;
	} m_meshes;
	struct {
		struct {
			types::texture::Texture* texture_pcx = nullptr;
			types::texture::Texture* ter1_pcx = nullptr;
		} source;
		types::texture::Texture* terrain = nullptr;
	} m_textures;

	// proxying because we can't create actors in this thread
	std::unordered_map< std::string, sprite_actor_t > m_sprite_actors = {};
	std::unordered_map< size_t, std::pair< std::string, types::Vec3 > > m_sprite_instances = {};
	size_t m_next_sprite_instance_id = 1;

	// these are needed on updates of tile(s)
	std::unordered_map< std::string, sprite_actor_t > m_sprite_actors_to_add = {};
	std::unordered_map< size_t, std::string > m_sprite_instances_to_remove = {};
	std::unordered_map< size_t, std::pair< std::string, types::Vec3 > > m_sprite_instances_to_add = {};

private:
	const int ITERATE_STATE_EVERY_N_TILES = 64;

	Game* m_game = nullptr;

	tile::Tiles* m_tiles = nullptr;
	MapState* m_map_state = nullptr;

	typedef std::vector< tile::Tile* > tiles_t;

	typedef std::vector< module::Module* > module_pass_t;
	typedef std::vector< module_pass_t > module_passes_t;
	module_passes_t m_modules; // before finalizing and deferred calls
	module_passes_t m_modules_deferred; // after finalizing and deferred calls

	void InitTextureAndMesh();
	void ProcessTiles( module_passes_t& module_passes, const tiles_t& tiles, MT_CANCELABLE );
	void LoadTiles( const tiles_t& tiles, MT_CANCELABLE );
	void FixNormals( const tiles_t& tiles, MT_CANCELABLE );

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

	tile::TileState* m_current_ts = nullptr;
	const tile::Tile* m_current_tile = nullptr;

};

}
}
}
