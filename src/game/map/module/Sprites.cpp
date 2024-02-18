#include <algorithm>

#include "Sprites.h"

#include "scene/actor/Sprite.h"

namespace game {
namespace map {
namespace module {

void Sprites::GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) {

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
    GenerateSprite( tile, ts, _name, s_consts.tc.ter1_pcx._texture, 0.4f + 0.001f * ( std::find( sprite_z_order.begin(), sprite_z_order.end(), _name ) - sprite_z_order.begin() ) )

#define FEATURE_SPRITE( _feature, _name, _texture ) \
    if ( tile->features & Tile::_feature ) { \
        SPRITE( _name, _texture ); \
    }

#define TERRAFORMING_SPRITE( _terraforming, _name, _texture ) \
    if ( tile->terraforming & Tile::_terraforming ) { \
        SPRITE( _name, _texture ); \
    }

	if ( tile->is_water_tile ) {
		FEATURE_SPRITE( F_GEOTHERMAL, "Geothermal", geothermal[ 0 ] );

		switch ( tile->bonus ) {
			case Tile::B_NUTRIENT: {
				SPRITE( "NutrientBonusSea", nutrient_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case Tile::B_ENERGY: {
				SPRITE( "EnergyBonusSea", energy_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case Tile::B_MINERALS: {
				SPRITE( "MineralsBonusSea", minerals_bonus_sea[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			default: {
				// nothing
			}
		}

		TERRAFORMING_SPRITE( T_FARM, "FarmSea", farm_sea[ 0 ] );
		TERRAFORMING_SPRITE( T_SOLAR, "SolarSea", solar_sea[ 0 ] );
		TERRAFORMING_SPRITE( T_MINE, "MineSea", mine_sea[ 0 ] );
	}
	else {
		switch ( tile->bonus ) {
			case Tile::B_NUTRIENT: {
				SPRITE( "NutrientBonusLand", nutrient_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case Tile::B_ENERGY: {
				SPRITE( "EnergyBonusLand", energy_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			case Tile::B_MINERALS: {
				SPRITE( "MineralsBonusLand", minerals_bonus_land[ m_map->GetRandom()->GetUInt( 0, 1 ) ] );
				break;
			}
			default: {
				// nothing
			}
		}
		FEATURE_SPRITE( F_URANIUM, "Uranium", uranium[ 0 ] );

		TERRAFORMING_SPRITE( T_CONDENSER, "Condenser", condenser[ 0 ] );

		TERRAFORMING_SPRITE( T_SOLAR, "SolarLand", solar_land[ 0 ] );

		// TODO: select based on nutrients yields instead of moisture
		TERRAFORMING_SPRITE( T_FARM, "FarmLand", farm_land[ tile->moisture ] );
		TERRAFORMING_SPRITE( T_SOIL_ENRICHER, "SoilEnricher", soil_enricher[ tile->moisture ] );

		TERRAFORMING_SPRITE( T_MINE, "MineLand", mine_land[ 0 ] );
		TERRAFORMING_SPRITE( T_MIRROR, "EchelonMirror", mirror[ 0 ] );
		TERRAFORMING_SPRITE( T_BOREHOLE, "ThermalBorehole", borehole[ 0 ] );

		TERRAFORMING_SPRITE( T_AIRBASE, "Airbase", airbase[ 0 ] );

	}

	FEATURE_SPRITE( F_MONOLITH, "Monolith", monolith[ 0 ] );

	TERRAFORMING_SPRITE( T_SENSOR, "Sensor", sensor[ 0 ] );

	if ( !tile->is_water_tile ) {
		TERRAFORMING_SPRITE( T_BUNKER, "Bunker", bunker[ 0 ] );
	}

	if ( !tile->is_water_tile ) {
		FEATURE_SPRITE( F_UNITY_POD, "UnityPodLand", unity_pod_land[ m_map->GetRandom()->GetUInt( 0, 2 ) ] );
	}
	else {
		FEATURE_SPRITE( F_UNITY_POD, "UnityPodSea", unity_pod_sea[ m_map->GetRandom()->GetUInt( 0, 2 ) ] );
	}

#undef FEATURE_SPRITE
#undef TERRAFORMING_SPRITE
#undef SPRITE

}

void Sprites::GenerateSprite( const Tile* tile, TileState* ts, const std::string& name, const Consts::pcx_texture_coordinates_t& tex_coords, const float z_index ) {
	TileState::sprite_t sprite = {};

	const auto& coords = tile->is_water_tile
		? ts->layers[ TileState::LAYER_WATER ].coords
		: ts->layers[ TileState::LAYER_LAND ].coords;

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
