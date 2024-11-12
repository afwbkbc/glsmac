#include <algorithm>

#include "Sprites.h"

#include "scene/actor/Sprite.h"
#include "game/backend/map/Map.h"
#include "game/backend/map/Consts.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/tile/TileState.h"
#include "game/backend/Random.h"

namespace game {
namespace backend {
namespace map {
namespace module {

void Sprites::GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) {

	for ( auto& sprite : ts->sprites ) {
		m_map->RemoveTerrainSpriteActorInstance( sprite.actor, sprite.instance );
	}
	ts->sprites.clear();

	const std::vector< std::string > sprite_z_order = {
		"Geothermal",
		"Uranium",

		"NutrientBonusLand",
		"NutrientBonusSea",
		"EnergyBonusLand",
		"EnergyBonusSea",
		"MineralsBonusLand",
		"MineralsBonusSea",

		"Condenser",

		"FarmLand",
		"FarmSea",
		"SolarLand",
		"SolarSea",
		"MineLand",
		"MineSea",

		"ThermalBorehole",
		"SoilEnricher",
		"EchelonMirror",

		"Monolith",

		"Airbase",
		"Sensor",
		"Bunker",

		"UnityPodSea",
		"UnityPodLand",
	};

#define SPRITE( _name, _texture ) \
    ASSERT( std::find( sprite_z_order.begin(), sprite_z_order.end(), _name ) != sprite_z_order.end(), "sprite '" _name "' not found in sprite z order" ); \
    GenerateSprite( tile, ts, _name, s_consts.tc.ter1_pcx._texture, 0.002f * ( std::find( sprite_z_order.begin(), sprite_z_order.end(), _name ) - sprite_z_order.begin() ) )

#define FEATURE_SPRITE( _feature, _name, _texture ) \
    if ( tile->features & tile::_feature ) { \
        SPRITE( _name, _texture ); \
    }

#define TERRAFORMING_SPRITE( _terraforming, _name, _texture ) \
    if ( tile->terraforming & tile::_terraforming ) { \
        SPRITE( _name, _texture ); \
    }

	if ( tile->is_water_tile ) {
		FEATURE_SPRITE( FEATURE_GEOTHERMAL, "Geothermal", geothermal[ 0 ] );

		switch ( tile->bonus ) {
			case tile::BONUS_NUTRIENT: {
				SPRITE( "NutrientBonusSea", nutrient_bonus_water[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case tile::BONUS_ENERGY: {
				SPRITE( "EnergyBonusSea", energy_bonus_water[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case tile::BONUS_MINERALS: {
				SPRITE( "MineralsBonusSea", minerals_bonus_water[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			default: {
				// nothing
			}
		}

		TERRAFORMING_SPRITE( TERRAFORMING_FARM, "FarmSea", farm_water[ 0 ] );
		TERRAFORMING_SPRITE( TERRAFORMING_SOLAR, "SolarSea", solar_water[ 0 ] );
		TERRAFORMING_SPRITE( TERRAFORMING_MINE, "MineSea", mine_water[ 0 ] );
	}
	else {
		switch ( tile->bonus ) {
			case tile::BONUS_NUTRIENT: {
				SPRITE( "NutrientBonusLand", nutrient_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case tile::BONUS_ENERGY: {
				SPRITE( "EnergyBonusLand", energy_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case tile::BONUS_MINERALS: {
				SPRITE( "MineralsBonusLand", minerals_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			default: {
				// nothing
			}
		}
		FEATURE_SPRITE( FEATURE_URANIUM, "Uranium", uranium[ 0 ] );

		TERRAFORMING_SPRITE( TERRAFORMING_CONDENSER, "Condenser", condenser[ 0 ] );

		TERRAFORMING_SPRITE( TERRAFORMING_SOLAR, "SolarLand", solar_land[ 0 ] );

		// TODO: select based on nutrients yields instead of moisture
		TERRAFORMING_SPRITE( TERRAFORMING_FARM, "FarmLand", farm_land[ tile->moisture ] );
		TERRAFORMING_SPRITE( TERRAFORMING_SOIL_ENRICHER, "SoilEnricher", soil_enricher[ tile->moisture ] );

		TERRAFORMING_SPRITE( TERRAFORMING_MINE, "MineLand", mine_land[ 0 ] );
		TERRAFORMING_SPRITE( TERRAFORMING_MIRROR, "EchelonMirror", mirror[ 0 ] );
		TERRAFORMING_SPRITE( TERRAFORMING_BOREHOLE, "ThermalBorehole", borehole[ 0 ] );

		TERRAFORMING_SPRITE( TERRAFORMING_AIRBASE, "Airbase", airbase[ 0 ] );

	}

	FEATURE_SPRITE( FEATURE_MONOLITH, "Monolith", monolith[ 0 ] );

	TERRAFORMING_SPRITE( TERRAFORMING_SENSOR, "Sensor", sensor[ 0 ] );

	if ( !tile->is_water_tile ) {
		TERRAFORMING_SPRITE( TERRAFORMING_BUNKER, "Bunker", bunker[ 0 ] );
	}

	if ( !tile->is_water_tile ) {
		FEATURE_SPRITE( FEATURE_UNITY_POD, "UnityPodLand", unity_pod_land[ m_map->GetRandom()->GetUInt( 0, 2 ) ] );
	}
	else {
		FEATURE_SPRITE( FEATURE_UNITY_POD, "UnityPodSea", unity_pod_water[ m_map->GetRandom()->GetUInt( 0, 2 ) ] );
	}

#undef FEATURE_SPRITE
#undef TERRAFORMING_SPRITE
#undef SPRITE

}

void Sprites::GenerateSprite( const tile::Tile* tile, tile::TileState* ts, const std::string& name, const pcx_texture_coordinates_t& tex_coords, const float z_index ) {
	tile::TileState::sprite_t sprite = {};

	const auto& coords = tile->is_water_tile
		? ts->layers[ tile::LAYER_WATER ].coords
		: ts->layers[ tile::LAYER_LAND ].coords;

	sprite.name = name;
	sprite.tex_coords = tex_coords;
	sprite.actor = m_map->GetTerrainSpriteActor( name, sprite.tex_coords, z_index );
	const types::Vec3 center = {
		( coords.left.x + coords.right.x ) / 2,
		( coords.top.y + coords.bottom.y ) / 2,
		( coords.left.z + coords.top.z + coords.right.z + coords.bottom.z ) / 4
	};
	sprite.instance = m_map->AddTerrainSpriteActorInstance(
		sprite.actor, {
			center.x,
			-center.y, // TODO: fix y inversion
			center.z
		}
	);

	ts->sprites.push_back( sprite );
}

}
}
}
}
